//Entrypoint file
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "../headers/cat.h"
#include "../headers/list.h"
#include  "../headers/preview.h"
#include "../headers/pwd.h"
#include "../headers/utils.h"
#include "../help/ascii.h"
#include "../headers/touch.h"
#include  "../headers/cd.h"
#include "../headers/mkdir.h"
#include "../help/help.h"

static void restore_terminal(void) {
  disable_raw();
  printf("\033[?25H\033[?10491");
  fflush(stdout);
}
static void handle_exit_signal(int signo) {
  (void)signo;
  restore_terminal();
  _exit(128 + signo);
}
static void install_signal_handlers(void) {
  struct sigaction sa = {0};
  sa.sa_handler = handle_exit_signal;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
}
static  void free_history(char **history, int his_len) {
  for (int i = 0; i < his_len; i++) {
    free(history[i]);
  }
}
static int parse_octal_mode(const char *text, mode_t *out) {
  char *end  = NULL;
  errno = 0;
  long val = strtol(text, &end, 8);
  if (errno !=0 || end == text || *end != '\0' || val < 0 || val > 07777) {
    return 1;
  }

  *out = (mode_t)val;
  return 0;

}
int main(void) {
  atexit(restore_terminal);
  install_signal_handlers();
  printf("\033[?251");
  fflush(stdout);
  print_ascii_name();
  const char*working =return_last_dir(print_workiing());
  if (print_workiing() == NULL) {
    fprintf(stderr, "Error with opening dir\n");
     return 1;
  }
  char*history[100] ={NULL};
  int hist_index = 0;
  int hist_len = 0;
  enable_raw();
  int runing = 1;
  while (runing) {
    char *line = read_command_line(history, &hist_index, &hist_len, working);
    if (line == NULL) {
      return 1;
    }
    char *argv[16];
    int argc = parse_line(line, argv, 16);
    if (argc == 0) {
       working = return_last_dir(print_workiing());
      continue;
    }
    disable_raw();
     int output_code = get_command_id(argv[0]);
    switch (output_code) {
      case CMD_preview: {
         struct  AppState st ={0};
         st.fs.cwd = malloc(PATH_MAX);
        if (!st.fs.cwd) {
          perror("malloc");
          break;
        }
        setvbuf(stdout, NULL, _IOFBF, 0);
        printf("\033[?1049h");
        printf("\033[?25l");
        fflush(stdout);
        if (!getcwd(st.fs.cwd, PATH_MAX)) {
          perror("getcwd");
          free(st.fs.cwd);
          printf("\033[?25h");
          printf("\033[?1049l");
          break;
        }
        st.fs.capacity = 32;
        st.fs.len = 0;
        st.fs.f_list = malloc(sizeof(FileEntry) * st.fs.capacity);
        if (!st.fs.f_list) {
          perror("malloc");
          free(st.fs.cwd);
          printf("\033[?25h");
          printf("\033[?1049l");
          break;
        }
        st.fs.len = list(&st);
        enable_raw();
        input_monitor(&st);
        disable_raw();
        printf("\033[?25h");
        printf("\033[?1049l");
        fflush(stdout);
        for (int i = 0; i < st.fs.len; i++) {
          free(st.fs.f_list[i].path);
        }
        free(st.fs.f_list);
        free(st.fs.cwd);
        free(st.fs.view);
        free(st.fs.enter_search);
        printf("\033[?25l");
        break;
      }
      case CMD_touch: {
         int start = 1;
         char cstamp[20] ={0};
        int flags = parse_touch_flags(argc, argv , &start);
        if (flags == -1) break;
        int atime = (flags & TOUCH_ATTIME) !=0;
        int mtime = (flags & TOUCH_MTTIME) !=0;
        int stamp = (flags & TOUCH_STAMP) !=0;
        if (stamp) {
           if (start >= argc) {
             fprintf(stderr, "touch:option requires an argument\n");
             break;

           }
           if (snprintf(cstamp, sizeof(cstamp), "%s", argv[start]) >= (int)sizeof(cstamp)) {
             fprintf(stderr, "touch:timestamp too long\n");
             break;
           }
          start++;
        }
        if (start >= argc) {
          fprintf(stderr, "touch:missing file operand\n");
          break;
        }
        for (int i = start; i < argc; i++) {
           touch_file(argv[i], cstamp, atime, stamp, mtime);
        }
        break;
      }
      case CMD_exit:
        fprintf(stdout, "exit in progress...\n");
        disable_raw();
        printf("\033[?25h");
        fflush(stdout);
        free_history(history, hist_len);
        runing = 0;
        break;
      case CMD_pwd: {
         const char *pwd_v;
         if ((pwd_v = print_workiing())!= NULL) {
           fprintf(stdout, "%s\n", pwd_v);
           break;
         }
        perror("pwd");
        break;

      }
      case CMD_clear:
        clear_screen();
        break;
      case CMD_ls: {
        int start = 1;
        int flags = parse_ls_flags(argc, argv , &start);
        int hidden = (flags & LS_A);
        if (start >= argc) {
          list_wd(".", hidden);
        }else {
          for (int i = start; i < argc; i++) {
             if (argc - start > 1) {
                printf("%s:\n", argv[i]);
             }
             list_wd(argv[i], hidden);
            if (i + i < argc) putchar('\n');
          }
        }
        break;
      }
      case CMD_cd: {
        int start = 0;
        int flags = parse_cd_args(argc, argv , &start);
        if (flags == -1) break;
        int help = (flags & CD_HL) !=0;
        if (help) {
          cd_help();
          break;
        }
        if (start >= argc) {
          cmd_cd(NULL);
          working = return_last_dir(print_workiing());
          break;
        }

      }
        case CMD_cat: {
         int start = 1;
        int flags  = parse_cat_flags(argc, argv, &start);
        if (flags == -1) break;
        int help = (flags & CAT_HL) !=0;
        if (help) {
          cat_help();
          break;
        }
        if (start >= argc) {
           fprintf(stderr, "cat:option requires an file operand \n");
          break;
        }
        for (int i = start; i < argc; i++) {
          if (cat_file(argv[i])!= 0) {
            fprintf(stderr, "Error: reading files: %s\n", argv[i]);
            break;
          }
        }
        break;
      }
      case CMD_mkdir: {
         int start = 1;
          int flags = parse_mkdir_flags(argc, argv , &start);
        if (flags == -1) break;
        int perms = (flags & MKDIR_M) != 0;
        int recursive = (flags & MKDIR_P) != 0;
        int verbose =(flags & MKDIR_V) != 0;
        mode_t mode_perms = 0 ;
        if (perms) {
          if (start >= argc) {
             fprintf(stderr, "mkdir:option requires an argument -- m\n");
            break;
          }
          if (parse_octal_mode(argv[start], &mode_perms)!=0) {
            fprintf(stderr, "mkdir:Invalid mode: %s\n", argv[start]);
            break;
          }
          start++;
        }
        if (start >= argc) {
          fprintf(stderr, "mkdir:missing operand\n");
          break;
        }
        for (int i = start; i < argc; i++) {
           create_dir(argv[i], recursive, mode_perms, verbose);
           working = return_last_dir(print_workiing());
        }
        break;
      }
        default:
            printf("command not found: %s\n", argv[0]);
           break;
    }
    enable_raw();

  }
  free_history(history, hist_len);
  return 0;
}