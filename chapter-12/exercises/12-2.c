#include <fcntl.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "../../lib/tlpi_hdr.h"

#define STATUS_FILE_MAX 1024
#define FILE_PATH_MAX 1024


#define ANSI_COLOR_RED     		"\x1b[31m"
#define ANSI_COLOR_GREEN   		"\x1b[32m"
#define ANSI_COLOR_YELLOW  		"\x1b[33m"
#define ANSI_COLOR_BLUE    		"\x1b[34m"
#define ANSI_COLOR_MAGENTA 		"\x1b[35m"
#define ANSI_COLOR_CYAN    		"\x1b[36m"
#define ANSI_BACKGROUND_CYAN 	"\x1b[46m"
#define ANSI_COLOR_RESET   		"\x1b[0m"


typedef struct Status {
    pid_t pid;
    char comm[128];
    char state;
    pid_t ppid;
    gid_t pgrp;
    struct Status *prev;
    struct Status *next;
    int flags;
} Status;


Status *rewindStatus(Status *status) {
    Status *sptr = status;
    while (sptr->prev != NULL) {
        sptr = sptr->prev;
    }

    return sptr;
}


void insertProcess(Status *previous, Status *current)
{
    if (current) {
        if (previous) {
            previous->next = current;
        }
        current->prev = previous;
    }
}


void freeAll(Status *status)
{
    if (status != NULL) {
        Status *sptr = rewindStatus(status);
        Status *prev;

        prev = sptr;
        sptr = sptr->next;

        while (sptr != NULL) {
            free(prev);
            prev = sptr;
            sptr = sptr->next;
        }

        free(prev);
    }
}


Status *getProcessStatus(pid_t pid)
{
    int fd;
    char filename[FILE_PATH_MAX] = {0};
    char stat_file[STATUS_FILE_MAX] = {0};

    Status *status = calloc(1, sizeof(Status));

    snprintf(filename, FILE_PATH_MAX, "/proc/%ld/stat", (long)pid);

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


void levelPadding(int level) {
    while (level--) printf("    ");
}


int hasChildren(Status *status)
{
    Status *sp = rewindStatus(status);

    while (sp) {
    	if (sp->ppid == status->pid && sp != status) {
    		return 1;
    	}

    	sp = sp->next;
    }

    return 0;
}


void pChild(Status *child)
{
	if (getpid() == child->pid) {
	    printf(
	        "%s[%-20sPID: %-10dPPID: %d]%s\n",
	        ANSI_BACKGROUND_CYAN,
	        child->comm,
	        child->pid,
	        child->ppid,
	        ANSI_COLOR_RESET
	    );
	} else {
	    printf(
	        "[%s%-20s%sPID: %s%-10d%sPPID: %s%d%s]\n",
	        ANSI_COLOR_CYAN,
	        child->comm,
	        ANSI_COLOR_RESET,
	        ANSI_COLOR_YELLOW,
	        child->pid,
	        ANSI_COLOR_RESET,
	        ANSI_COLOR_GREEN,
	        child->ppid,
	        ANSI_COLOR_RESET
	    );
	}
}


void pParent(Status *parent)
{
    printf(
        "%s[%-20sPID: %-10dPPID: %d]%s\n",
        ANSI_COLOR_RED,
        parent->comm,
        parent->pid,
        parent->ppid,
        ANSI_COLOR_RESET
    );
}


void drawChildren(Status *parent, Status *all, int level)
{
    Status *ap = rewindStatus(all);

    while (ap) {

        if (ap->ppid == parent->pid && ap != parent) {

            levelPadding(level);

            if (hasChildren(ap)) {
            	pParent(ap);
                drawChildren(ap, all, level + 1);
            } else {
            	pChild(ap);
            }
        }

        ap = ap->next;
    }
}


void draw(Status *status)
{
    Status *sptr = rewindStatus(status);

    while (sptr) {
        if (sptr->ppid == 0) {
        	pParent(sptr);
            drawChildren(sptr, status, 1);
        }

        sptr = sptr->next;
    }
}


int main(int argc, char *argv[])
{
    DIR *dirp;
    struct dirent *dp;

    Status *status;
    Status *previous = NULL;

    dirp = opendir("/proc/");

    while ((dp = readdir(dirp)) != NULL) {

        if (isNumericString(dp->d_name)) {

            status = getProcessStatus(atoi(dp->d_name));

            if (status != NULL) {
                insertProcess(previous, status);
                previous = status;
            }

        }

    }

    draw(status);

    freeAll(previous);
    closedir(dirp);

    exit(EXIT_SUCCESS);
}
