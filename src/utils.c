#include "../headers/utils.h"

#include <ctype.h>
#include <stdio.h>
#include <sys/stat.h>
int dir_exists(const char *path) {
    struct stat s;
    return (stat(path, &s) == 0) && S_ISDIR(s.st_mode);
}
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