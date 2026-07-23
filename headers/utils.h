
#ifndef FILEEXPLORER_UTILS_H
#define FILEEXPLORER_UTILS_H
#include <stdbool.h>
enum Commands {CMD_exit, CMD_pwd, CMD_ls, CMD_cd, CMD_clear, CMD_cat, CMD_rm, CMD_mkdir, CMD_touch, CMD_preview};
typedef  struct {
    const char *name;
    int id;
}Cmd;
extern Cmd cmds[];
int dir_exists(const char *path);
void print_sentized_text(const char *text, bool preserve_tabs );
char *skip_spaces(char *s);
void disable_raw(void);
void enable_raw(void);
void *read_command_line(char **history, int *index,int *history_len, const char*working_dir);
const char *return_last_dir(const char *working);
void rredraw(const char*buf, const char*working_dir, int cursor);
void delete_world(  char*buf, int *cursor, int*len);
int parse_line(char*line, char**argv, int max);
int get_command_id(const char *command);
void clear_screen(void);
#endif //FILEEXPLORER_UTILS_H
