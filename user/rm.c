#include <lib.h>

void usage() {
    debugf("usage: rm [-r | -f] <file | directory>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        usage();
        return 1;
    }
    char *path;
    int recursive = 0;
    int force = 0;
    if (argc == 2) {
        // rm file / dir
        path = argv[1];
    } else {
        if (strcmp(argv[1], "-r") == 0) {
            // rm -r dir
            recursive = 1;
        } else if (strcmp(argv[1], "-rf") == 0) {
            // rm -rf dir
            recursive = 1;
            force = 1;
        } else {
            usage();
            return 1;
        }
        path = argv[2];
    }
    int fd_num;
    if ((fd_num = open(path, O_RDONLY)) < 0) {
        // path does not exist
        if (!force) {
            debugf("rm: cannot remove '%s': No such file or directory\n", path);
        }
        close(fd_num);
        return 1;
    }
    // path exists, get its type
    struct Fd *fd;
    int r;
    if ((r = fd_lookup(fd_num, &fd)) < 0) {
        return 1;
    }
    struct Filefd *filefd = (struct Filefd *)fd;
    int type = filefd->f_file.f_type;
    if (type == FTYPE_DIR) {
        if (argc == 2) {
            // rm dir
            debugf("rm: cannot remove '%s': Is a directory\n", path);
            return 1;
        } else if (argc == 3 && recursive) {
            // rm -r dir / rm -rf dir
            remove(path);
        }
    } else {
        // rm file
        remove(path);
    }
    return 0;
}