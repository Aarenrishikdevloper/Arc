#ifndef TOUCH_H
#define TOUCH_H
#define  TOUCH_ATTIME 1
#define  TOUCH_MTTIME 2
#define  TOUCH_STAMP 4

int atoi_n(const char *s, int pos, int len);
int touch_file(const char *path, char *stamp, int atime, int has_stamp,int mtime);
int parse_touch_flags(int argc, char **argv, int *start);

#endif
