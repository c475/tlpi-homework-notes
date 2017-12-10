#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include "lib.h"
#include "file_perms.h"
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sysmacros.h>

#define STATUS_FILE_MAX 1024


char *_strtok(char *content, char delim)
{
	static char _strtok_content_buffer[_STRTOK_CONTENT_BUFFER_MAX];
	static char _strtok_token_buffer[_STRTOK_TOKENR_MAX];

	static char *_strtok_token = NULL;
	static char *_strtok_tbp;

	if (content != NULL) {
		_strtok_token = NULL;
		_strtok_tbp = NULL;
		memset(_strtok_content_buffer, 0, _STRTOK_CONTENT_BUFFER_MAX);
		memcpy(_strtok_content_buffer, content, _STRTOK_CONTENT_BUFFER_MAX);
	}

	if (_strtok_token == NULL) {
		_strtok_token = _strtok_content_buffer;
		_strtok_tbp = _strtok_token;
	}

	while (*_strtok_tbp && *_strtok_tbp != delim) _strtok_tbp++;

	if (*_strtok_tbp == '\0') {
		return NULL;
	}

	memset(_strtok_token_buffer, 0, _STRTOK_TOKENR_MAX);

	*_strtok_tbp = '\0';
	snprintf(_strtok_token_buffer, _STRTOK_TOKENR_MAX-1, "%s", _strtok_token);
	*_strtok_tbp++ = delim;
	_strtok_token = _strtok_tbp;

	return _strtok_token_buffer;
}


int isNumericString(char *str)
{
	char *strp = str;

	if (strp == NULL || *strp == '\0') {
		return 0;
	}

	for (; *strp; strp++) {
		if (!(*strp >= '0' && *strp <= '9')) {
			return 0;
		}
	}

	return 1;
}


char *userNameFromId(uid_t uid)
{
	struct passwd *pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}


uid_t userIdFromName(const char *name)
{
	struct passwd *pwd;

	if (name == NULL || *name == '\0') {
		return -1;
	}

	pwd = getpwnam(name);
	if (pwd == NULL) {
		return -1;
	}

	return pwd->pw_uid;
}


char *groupNameFromId(gid_t gid)
{
	struct group *grp = getgrgid(gid);
	return grp == NULL ? NULL : grp->gr_name;
}


gid_t groupIdFromName(const char *name)
{
	struct group *grp;

	if (name == NULL || *name == '\0') {
		return -1;
	}

	grp = getgrnam(name);
	if (grp == NULL) {
		return -1;
	}

	return grp->gr_gid;
}


struct WindowDimensions getWindowSize(void)
{
	struct winsize ws;
	struct WindowDimensions win;
	ioctl(0, TIOCGWINSZ, &ws);
	win.width = ws.ws_col;
	win.height = ws.ws_row;
	return win;
}


Status *getProcessStatus(pid_t pid)
{
    int fd;
    char filename[PATH_MAX] = {0};
    char stat_file[STATUS_FILE_MAX] = {0};

    Status *status = calloc(1, sizeof(Status));

    snprintf(filename, PATH_MAX, "/proc/%ld/stat", (long)pid);

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    if (read(fd, stat_file, STATUS_FILE_MAX) == -1) {
        return NULL;
    }

    close(fd);

    sscanf(stat_file, "%d %s %c %d %d",
        &(status->pid),
        status->comm,
        &(status->state),
        &(status->ppid),
        &(status->pgrp)
    );

    status->prev = NULL;
    status->next = NULL;

    return status;
}


int endswith(char *buffer, char *suffix)
{
	char *substring;

	if (!buffer || !suffix || !(*buffer) || !(*suffix)) {
		return 0;
	}

	substring = strstr(buffer, suffix);

	if (substring == NULL) {
		return 0;
	}

	substring += strlen(suffix);

	return *substring == '\0';
}


int startswith(char *buffer, char *prefix)
{
	if (!buffer || !prefix || !(*buffer) || !(*prefix)) {
		return 0;
	}

	return strncmp(buffer, prefix, strlen(prefix)) == 0;
}


void displayStatInfo(const struct stat *sb)
{
    printf("File type:            ");

    switch(sb->st_mode & S_IFMT) {
        case S_IFREG:
            printf("regular file\n");
            break;
        case S_IFDIR:
            printf("directory\n");
            break;
        case S_IFCHR:
            printf("character device\n");
            break;
        case S_IFBLK:
            printf("block device\n");
            break;
        case S_IFLNK:
            printf("symbolic (soft) link\n");
            break;
        case S_IFIFO:
            printf("FIFO or pipe\n");
            break;
        case S_IFSOCK:
            printf("socket\n");
            break;
        default:
            printf("?unknown?\n");
            break;
    }

    printf(
        "Device containing i-node: major=%ld   minor=%ld\n",
        (long)major(sb->st_dev),
        (long)minor(sb->st_dev)
    );

    printf("I-node number:        %ld\n", sb->st_ino);

    printf(
        "Mode:                 %lo (%s)\n",
        // 700, 777, 600, etc
        (unsigned long)sb->st_mode,
        filePermStr(sb->st_mode, 0)
    );

    // special bits
    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX)) {
        printf(
            "    special bits set: %s%s%s\n",
            sb->st_mode & S_ISUID ? "set-UID " : "",
            sb->st_mode & S_ISGID ? "set-GID " : "",
            sb->st_mode & S_ISVTX ? "sticky "  : ""
        );
    }

    printf(
        "Number of (hard) links:    %ld\n",
        (long)sb->st_nlink
    );

    printf(
        "Ownership:              UID=%ld   GID=%ld\n",
        (long)sb->st_uid,
        (long)sb->st_gid
    );

    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
        printf(
            "Device number (st_rdev): major=%ld; minor=%ld\n",
            (long)major(sb->st_rdev),
            (long)minor(sb->st_rdev)
        );
    }

    printf("File size:               %lld bytes\n", (long long)sb->st_size);
    printf("Optimal I/O block size:  %ld bytes\n",  (long)sb->st_blksize);
    printf("512B blocks allocated:   %lld\n",       (long long)sb->st_blocks);
    printf("Last file access:        %s",           ctime(&sb->st_atime));
    printf("Last file modification:  %s",           ctime(&sb->st_mtime));
    printf("Last status change:      %s",           ctime(&sb->st_ctime));
}
