#include <lib.h>

void usage() {
    debugf("usage: touch <file>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        return 1;
    }
    int fd;
    if ((fd = open(argv[1], O_RDONLY)) >= 0) {
        close(fd);
        return 1;
    }
    if (create(argv[1], FTYPE_REG) < 0) {
        debugf("touch: cannot touch '%s': No such file or directory\n",argv[1]);
        close(fd);
        return 1;
    }
    return 0;
}