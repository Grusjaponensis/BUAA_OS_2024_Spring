#include <lib.h>

#define P_OFF   0
#define P_ON    1

void usage() {
    debugf("usage: mkdir [-p] <directory_name>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        usage();
        return 1;
    }

    char *path;
    int fd;
    int mode = P_OFF;
    if (argc == 2) {
        // mkdir <dirname>
        path = argv[1];
    } else {
        // mkdir -p <dirname>
        mode = P_ON;
        path = argv[2];
    }

    if ((fd = open(path, O_RDONLY)) >= 0) {
        debugf("mkdir: cannot create directory '%s': File exists\n", path);
        close(fd);
        return 1;
    } else {
        // dir not exist
        if (create(path, FTYPE_DIR) != 0) {
            if (mode != P_ON) {
                debugf("mkdir: cannot create directory '%s': No such file or directory\n", path);
                close(fd);
                return 1;
            }
            int len = strlen(path);
            char path_create[MAXPATHLEN] = {0};
            for (int i = 0; i < len; i++) {
                if (path[i] == '/') {
                    path_create[i] = 0;
                    create(path_create, FTYPE_DIR);
                }
                path_create[i] = path[i];
            }
        }
    }
    return 0;
}