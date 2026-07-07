
#ifndef FILEEXPLORER_UTILS_H
#define FILEEXPLORER_UTILS_H
#include <stdbool.h>
int dir_exists(const char *path);
void print_sentized_text(const char *text, bool preserve_tabs );
char *skip_spaces(char *s);
#endif //FILEEXPLORER_UTILS_H
