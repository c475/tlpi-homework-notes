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


// ugly, and not quite done (no numfd support, havent finished struct FTW). a small failure. this one is difficult.
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



/*
    vanilla nftw() with no flags beats this one by quite a bit
    it is very difficult to detect a symlink loop (efficiently) if the looping point exists deep enough in a subtree...
    which is why "ftw_skip" exists to sort of mitigate the problem, but it will not always work, just for this environment.
    it can cause nftw() to run forever, or until its stack or the heap overflows

    big problem is memory allocation...

    $ valgrind ./18-8 /
    ==3864== Memcheck, a memory error detector
    ==3864== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
    ==3864== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
    ==3864== Command: ./18-8 /
    ==3864== 
    ==3864== 
    ==3864== HEAP SUMMARY:
    ==3864==     in use at exit: 0 bytes in 0 blocks
    ==3864==   total heap usage: 665,390 allocs, 665,390 frees, 2,387,567,180 bytes allocated
    ==3864== 
    ==3864== All heap blocks were freed -- no leaks are possible
    ==3864== 
    ==3864== For counts of detected and suppressed errors, rerun with: -v
    ==3864== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)


    $ valgrind ./18-7 /
    ==3868== Memcheck, a memory error detector
    ==3868== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
    ==3868== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
    ==3868== Command: ./18-7 /
    ==3868== 
    ==3868== 
    ==3868== HEAP SUMMARY:
    ==3868==     in use at exit: 0 bytes in 0 blocks
    ==3868==   total heap usage: 47,811 allocs, 47,811 frees, 1,569,952,864 bytes allocated
    ==3868== 
    ==3868== All heap blocks were freed -- no leaks are possible
    ==3868== 
    ==3868== For counts of detected and suppressed errors, rerun with: -v
    ==3868== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)


    $ time ./18-8 /

    real    0m1.291s
    user    0m0.368s
    sys 0m0.920s


    $ time ./18-7 /

    real    0m0.567s
    user    0m0.048s
    sys 0m0.512s


    Done trying with this one.
*/