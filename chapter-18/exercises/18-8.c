#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../../lib/tlpi_hdr.h"


struct FTW2 {
    int base;    // offset to basename part of pathname
    int level;   // depth of file within tree traversal
};


struct Book {
    dev_t mount;
    struct FTW2 state;
};


static struct Book FTW_BOOK = {0};


#define FTW_D               0x01
#define FTW_DNR             0x02
#define FTW_DP              0x04
#define FTW_F               0x08
#define FTW_NS              0x10
#define FTW_SL              0x20
#define FTW_SLN             0x80

#define FTW_CHDIR           0x2000
#define FTW_DEPTH           0x4000
#define FTW_MOUNT           0x8000
#define FTW_PHYS            0x10000

#define FTW_ACTIONRETVAL    0x100
#define FTW_CONTINUE        0x200
#define FTW_SKIP_SIBLINGS   0x400
#define FTW_SKIP_SUBTREE    0x800
#define FTW_STOP            0x1000


int somefunc(const char *pathname, const struct stat *sb, int type, struct FTW2 *ftwp)
{
    switch (type) {

        case FTW_D:
            printf("FTW_D: %s\n", pathname);
            break;

        case FTW_DNR:
            printf("FTW_DNR: %s\n", pathname);
            break;

        case FTW_DP:
            printf("FTW_DP: %s\n", pathname);
            break;

        case FTW_F:
            printf("FTW_F: %s\n", pathname);
            break;

        case FTW_NS:
            printf("FTW_NS: %s\n", pathname);
            break;

        case FTW_SL:
            printf("FTW_SL: %s\n", pathname);
            break;

        case FTW_SLN:
            printf("FTW_SLN: %s\n", pathname);
            break;
    }

    return 0;
}


static inline int is_dir(const char *dirname, struct stat *statbuf)
{
    struct stat statbuf2;

    if (dirname == NULL && statbuf == NULL) {
        return 0;
    }

    if (statbuf == NULL) {
        if (stat(dirname, &statbuf2) == -1) {
            return 0;
        }

        return (statbuf2.st_mode & S_IFMT) == S_IFDIR;
    }

    return (statbuf->st_mode & S_IFMT) == S_IFDIR;
}


static inline int getstat(const char *pathname, struct stat *statbuf, int flags)
{
    if (flags & FTW_PHYS) {
        if (lstat(pathname, statbuf) == -1) {
            return -1;
        }
    } else {
        if (stat(pathname, statbuf) == -1) {
            return -1;
        }
    }

    return 0;
}


static inline int ftw_file_type(const char *pathname, const struct stat *statbuf)
{
    switch (statbuf->st_mode & S_IFMT) {

        case S_IFLNK:
            return access(pathname, F_OK) != -1 ? FTW_SL : FTW_SLN;

        case S_IFDIR:
            return FTW_D;

        default:
            return FTW_F;

    }

    return FTW_F;
}


static inline int ftw_skip(const char *pathname)
{
    for (; *pathname; pathname++) {
        if (strcmp(pathname, "/dev/fd") == 0) {
            return 1;
        }

        if (strcmp(pathname, "/proc") == 0) {
            return 1;
        }

        if (strcmp(pathname, "/sys") == 0) {
            return 1;
        }

        if (strcmp(pathname, "/usr/bin/X11") == 0) {
            return 1;
        }
    }

    return 0;
}


static inline void ftw_clean_chdir(int fd, int flags)
{
    if (flags & FTW_CHDIR) {
        fchdir(fd);
        close(fd);
    }
}


// ugly, and not quite done (no numfd support, havent finished struct FTW)
int t_nftw(const char *dirname, int (*func) (const char *pathname, const struct stat *sbuf, int type, struct FTW2 *ftwb), 
    int numfd, int flags)
{
    DIR *dirp;
    struct stat statbuf;
    struct dirent *direntp;
    size_t path_len;
    char *path;
    int ftype;
    int func_val;
    int nftw_val;
    int skip_subtree;
    int skip_mount;
    int postorder;
    int fd;

    path = NULL;
    skip_subtree = 0;
    skip_mount = 0;
    postorder = flags & FTW_DEPTH;
    fd = -1;

    memset(&statbuf, 0, sizeof(struct stat));

    if (ftw_skip(dirname)) {
        return 0;
    }

    if (!is_dir(dirname, NULL)) {
        return 0;
    }

    if (flags & FTW_CHDIR) {
        fd = open(".", O_RDONLY);
        if (fd == -1) {
            return 0;
        }

        chdir(dirname);

        dirp = opendir(".");

    } else {
        dirp = opendir(dirname);
    }

    // can't read directory
    if (dirp == NULL) {
        func(dirname, NULL, FTW_DNR, NULL);
        ftw_clean_chdir(fd, flags);
        return 0;
    }

    for (;;) {

        direntp = readdir(dirp);
        if (direntp == NULL) {
            break;
        }

        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) {
            continue;
        }

        path_len = strlen(dirname) + strlen(direntp->d_name) + 2;
        path = calloc(1, path_len);
        if (path == NULL) {
            ftw_clean_chdir(fd, flags);
            return 0;
        }

        snprintf(path, path_len, strcmp(dirname, "/") == 0 ? "%s%s" : "%s/%s", dirname, direntp->d_name);

        if (getstat(path, &statbuf, flags) == -1) {
            ftype = FTW_NS;
        } else {
            ftype = ftw_file_type(path, &statbuf);
        }

        if (postorder == 0) {
            func_val = func(path, &statbuf, ftype, 0);
        }

        if ((flags & FTW_ACTIONRETVAL) && func_val != FTW_CONTINUE && postorder == 0) {

            if (func_val == FTW_STOP) {
                ftw_clean_chdir(fd, flags);
                free(path);
                closedir(dirp);
                return FTW_STOP;
            }

            if (func_val == FTW_SKIP_SIBLINGS) {
                ftw_clean_chdir(fd, flags);
                free(path);
                break;
            }

            if (func_val == FTW_SKIP_SUBTREE) {
                skip_subtree = 1;
            }

        } else if (postorder == 0) {

            if (func_val != 0) {
                ftw_clean_chdir(fd, flags);
                free(path);
                closedir(dirp);
                return func_val;
            }

        }

        if (is_dir(NULL, &statbuf)) {

            if (flags & FTW_MOUNT) {
                if (FTW_BOOK.mount == 0) {
                    FTW_BOOK.mount = statbuf.st_dev;
                }

                if (FTW_BOOK.mount != statbuf.st_dev) {
                    skip_mount = 1;
                }
            }

            if (skip_subtree == 0 && skip_mount == 0) {

                nftw_val = t_nftw(path, func, numfd, flags);

                if (nftw_val != 0) {
                    ftw_clean_chdir(fd, flags);
                    free(path);
                    closedir(dirp);
                    return nftw_val;
                }
            }
        }

        if (postorder) {
            func_val = func(path, &statbuf, ftype, 0);

            // DNRY but clean up later
            if ((flags & FTW_ACTIONRETVAL) && func_val != FTW_CONTINUE) {

                if (func_val == FTW_STOP) {
                    ftw_clean_chdir(fd, flags);
                    free(path);
                    closedir(dirp);
                    return FTW_STOP;
                }

                if (func_val == FTW_SKIP_SIBLINGS) {
                    ftw_clean_chdir(fd, flags);
                    free(path);
                    break;
                }

            } else {

                if (func_val != 0) {
                    ftw_clean_chdir(fd, flags);
                    free(path);
                    closedir(dirp);
                    return func_val;
                }

            }
        }

        free(path);
    }


    ftw_clean_chdir(fd, flags);

    closedir(dirp);

    return 0;
}


int main(int argc, char *argv[])
{
    t_nftw(argv[1], somefunc, 10, 0);
    exit(EXIT_SUCCESS);
}
