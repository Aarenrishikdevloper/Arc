#include "../headers/utils.h"
#include <termios.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
Cmd cmds[] = {
    {"exit", CMD_exit},
    {"pwd", CMD_pwd},
    {"ls", CMD_ls},
    {"cd", CMD_cd},
    {"clear", CMD_clear},
    {"cat", CMD_cat},
    {"rm", CMD_rm},
    {"mkdir", CMD_mkdir},
    {"touch", CMD_touch},
    {"preview", CMD_preview}
};
int dir_exists(const char *path) {
    struct stat s;
    return (stat(path, &s) == 0) && S_ISDIR(s.st_mode);
}
static int raw_enabled = 0;
struct termios orig;
void print_sentized_text(const char *text, bool preserve_tabs ) {
    for (const unsigned char*p =(const unsigned char*)text; *p!= '\0'; ++p) {
        if (*p == '\n') {
            putchar(('\n'));
            continue;
        }
        if (*p == '\t' && preserve_tabs) {
            putchar('\t');
            continue;
        }
        if (isprint(*p)) {
            putchar(*p);
        }else {
            putchar('?');
        }
    }
}
char *skip_spaces(char *s) {
    while (*s == ' ' || *s == '\t') s++;
    return s;
}
void enable_raw(void) {
    if (raw_enabled)return;
     if (tcgetattr(STDIN_FILENO, &orig) == -1) {
         return;
     }
    struct  termios raw = orig;
    raw.c_lflag &= ~(ECHO |ICANON | IEXTEN );
    raw.c_iflag &= ~(IXON |ICRNL);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0) {
        raw_enabled = 1;
    }

}
void disable_raw(void) {
    if (!raw_enabled) {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
    raw_enabled = 0;
}
const char *return_last_dir(const char *working) {
    const char*last = strrchr(working, '/');
    return (last && *(last+1))?last+1:working;
}
void rredraw(const char *buf, const char * working_dir, int cursor) {
    int len =(int) strlen(buf);
    printf("\r\033[K%s> ", working_dir );
    for (int i = 0; i < len; i++) {
        if (i == cursor) {
            printf("\033[7m%c\033[0m", buf[i]);
        }else putchar(buf[i]);
    }
    if (cursor == len) {
        printf("\033[7m \033[0m");
    }
    fflush(stdout);
}
 void delete_world(char*buf, int*curosr,int*len) {
    if (curosr == 0)return;
    int i = *curosr;
    while (i >0 && buf[i-1] == ' ') i--;
    while (i >0 && buf[i-1] == ' ') i--;
    int remove = *curosr -1;

    memmove(buf+1, buf + *curosr, *len - *curosr);
    *len -= remove;
    *curosr = i;
    buf[*len] = '\0';
}
void *read_command_line(char **history, int *index,int *history_len, const char*working_dir) {
     int cursor = 0;
     static char buf[1024];
      int len =0;
     buf[0] = '\0';
    rredraw(buf, working_dir, cursor);
    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1)<= 0) continue;
        if ( c == 23) {
            delete_world(buf, &cursor,&len);
            rredraw(buf, working_dir, cursor);
            continue;
        }
        if (c == 21) {
            cursor = 0;
            buf[0] = '\0';
            len = 0;
            rredraw(buf, working_dir, cursor);
            continue;
        }
        if (c == '\n' || c == '\r') {
            buf[len] = '\0';
            if (len > 0) {
                 if (*history_len == 100) {
                     free(history[0]);
                     memmove(history, history+1, sizeof(history[0]) *99);
                     history[99] = NULL;
                     *history_len = 99;
                 }
                 history[*history_len] = strdup(buf);
                 if (history[*history_len]) {
                     (*history_len)++;
                 }
            }
            *index = *history_len;
            printf("\n");
            return buf;
        }
        if ((c == 127 || c== 8) && cursor > 0) {
            memmove(buf+cursor - 1, buf+cursor, len-cursor);
            cursor--;
            len--;
            buf[len] = '\0';
            rredraw(buf, working_dir, cursor);
        }
        else if ( c== '\x1b') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) continue;
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) continue;
            if (seq[1] == 'A') {
                if (*index > 0) {
                    (*index)--;
                    if (history[*index]) {
                        strcpy(buf, history[*index]);
                    }else {
                        buf[0] = '\0';
                    }
                    len = cursor =(int) strlen(buf);
                    rredraw(buf, working_dir, cursor);
                }
            }
            if (seq[0] == 'B') {
                if (*index < (*history_len) -1) {
                    (*index)++;
                    if (history[*index]) {
                        strcpy(buf, history[*index]);
                    }else {
                        buf[0] = '\0';
                    }
                    len = cursor = (int)strlen(buf);
                }else {
                    *index = *history_len;
                     len = cursor = 0;
                    buf[0] = '\0';

                }
                rredraw(buf, working_dir, cursor);
            }
            if (seq[1] == 'D' && cursor > 0) {
                cursor--;
                rredraw(buf, working_dir, cursor);
            }
            if (seq[0] == 'C' && cursor < len) {
                cursor++;
                rredraw(buf, working_dir, cursor);
            }
        }
        if (c >= 32 && c< 127 && len <(int)sizeof(buf)-1) {
            memmove(buf+cursor+1, buf+cursor, len-cursor);
            buf[cursor] = c;
            cursor++;
            len++;
            buf[len] = '\0';
            rredraw(buf, working_dir, cursor);
        }
    }

}
int parse_line(char*line, char**argv, int max) {
    int argc = 0;
    char *src = line;
    char *dst  = line;
    line[strcspn(line, "\r\n")] = '\0';
    while (*src && argc < max -1) {
        while (*src == ' ' || *src == '\t') src++;
        if (!*src) break;
        char quote = '\0';
        argv[argc++] = dst;
       while (*src) {
           if (quote) {
               if (*src == '\\' && quote == '"' && src[1] != '\0') {
                   src++;
                   *dst++ = *src++;
                   continue;
               }
               if (*src == quote) {
                   src++;
                   quote = '\0';
                   continue;
               }
               *dst++ = *src++;
               continue;
           }
           if (*src == '\'' || *src == '"') {
               quote = *src++;
               continue;
           }
           if (*src == '\\' && src[1] !='\0') {
               src++;
               *dst++ = *src++;
               continue;
           }
           if (*src == ' ' || *src == '\t' ) {
               break;
           }
           *dst++ = *src++;
       }
        char*next = src;
        while (*next == ' ' || *next == '\t') next++;
        *dst = '\0';
        dst++;
        src = next;
    }
    argv[argc] = NULL;
    return argc;
}
int get_command_id(const char *command) {
    if (!command || command[0] == '\0') {
        return -10;
    }
    for (int i =0; i <10; i++) {
        if (strcmp(command, cmds[i].name) == 0) {
            return cmds[i].id;
        }
    }
    return -1;
}
void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}