#include "../headers/rm.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int ask_rm_confirmation(const char*prompt) {
    char ans[16];
    fprintf(stdout, "%s [y|n]", prompt);
    fflush(stdout);
    if (!fgets(ans, sizeof(ans), stdin)) {
        return  0;
    }
    return ans[0] == 'y' || ans[0] == 'Y';
}
int remove_item(const char *name, int confirm, int recursive, int force, int verbose) {
    struct stat st;

    if (confirm == 1) {
        char prompt[PATH_MAX + 32];
        snprintf(prompt, sizeof(prompt), "remove %s", name);
        if (!ask_rm_confirmation(prompt))   // pass prompt, not 0
            return 0;
    }

    if (lstat(name, &st) == -1) {           // fixed: check for failure (-1)
        if (force && errno == ENOENT) {
            return 0;
        }
        if (!force) perror("rm");
        return 1;
    }

    if (S_ISDIR(st.st_mode)) {
        if (recursive) {
            if (remove_dir(name) != 0) {
                return 1;
            }
            if (verbose) {
                printf("rm: remove %s\n", name);
            }
            return 0;
        }
        if (rmdir(name) != 0) {
            if (!force) perror("rm");
            return 1;
        }
        if (verbose) {
            printf("directory delete %s\n", name);
        }
        return 0;
    }

    if (unlink(name) != 0) {
        if (!force) perror("rm");
        return 1;
    }
    if (verbose) {
        printf("unlink %s\n", name);
    }
    return 0;
}

int remove_dir(const char *name) {
    DIR *w_dir = opendir(name);
    if (!w_dir) {
        fprintf(stderr, "rm: cannot open %s\n", name);
        return 1;
    }
    struct  dirent *ent;
    while ((ent = readdir(w_dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 ||
            strcmp(ent->d_name, "..") == 0)  continue;
        char fullpath[PATH_MAX];
        if (snprintf(fullpath ,sizeof(fullpath), "%s/%s", name, ent->d_name) >= (int)sizeof(fullpath)) {
            fprintf(stderr, "rm: path too long\n");
            closedir(w_dir);
            return 1;
        }
        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror("rm");
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
                if (remove_dir(fullpath) != 0) {
                    closedir(w_dir);
                    return 1;
                }
        }else {
            if (unlink(fullpath) != 0) {
                perror("unlinked");
                closedir(w_dir);
                return 1;
            }
        }

        // Process other files
    }
    closedir(w_dir);
    if (rmdir(name) == -1) {
        perror("rmdir");
        return 1;
    }
    return 0;


}
int parse_rm_flags(int argc, char **argv, int*start) {
    int flags = 0;
    int i = 1;
    while (i < argc && argv[i][0] == '-' && argv[i][1] != '\0') {
        for (int j =1; argv[i][j] != '\0'; j++) {
            char c = argv[i][j];
            if (c == 'i') flags |= RM_I;
            else if (c == 'r' || c == 'R') flags |= RM_R;
            else if (c == 'f') flags |= RM_F;
            else if (c == 'I') flags |= RM_INTER;
            else if (c == 'v') flags |= RM_V;
            else {
                fprintf(stderr, "rm: unknown flag -%c\n", c);
                return -1;
            }

        }
        i++;
    }
    *start = i;
    return  flags;
}
