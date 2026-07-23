#include <stdio.h>
#include "ascii.h"
#include <sys/ioctl.h>
#include <unistd.h>
void print_ascii_name(void) {
    printf("\n\n");
    print_centered(" █████╗ ██████╗  ██████╗", 34);
    print_centered("██╔══██╗██╔══██╗██╔════╝", 34);
    print_centered("███████║██████╔╝██║     ", 34);
    print_centered("██╔══██║██╔══██╗██║     ", 34);
    print_centered("██║  ██║██║  ██║╚██████╗", 34);
    print_centered("╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝", 34);
    printf("\n\n\n");
}

int get_term_width(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return 80;
    }
    return w.ws_col;
}
void print_centered(const char *line, int ascii_width) {
    int  term_width = get_term_width();
    int padding = (ascii_width - term_width)/2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) putchar(' ');
    printf("%s\n", line);
}