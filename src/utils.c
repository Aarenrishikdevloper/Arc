#include "../headers/utils.h"
#include <sys/stat.h>
int dir_exists(const char *path) {
    struct stat s;
    return (stat(path, &s) == 0) && S_ISDIR(s.st_mode);
}