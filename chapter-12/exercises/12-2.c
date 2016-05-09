#include <fcntl.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "../../lib/tlpi_hdr.h"

#define STATUS_FILE_MAX 1024
#define FILE_PATH_MAX 1024


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


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


pid_t getMinPPid(Status *status)
{
	Status *sptr = rewindStatus(status);
	pid_t min;
	int match = 0;

	if (sptr == NULL) {
		return -1;
	}

	while (sptr) {
		if (sptr->flags ^ 1) {
			match = 1;
			min = sptr->ppid;
		}

		sptr = sptr->next;
	}

	sptr = rewindStatus(status);

	while (sptr) {
		if (sptr->ppid < min || sptr->ppid == min) {
			if (sptr->flags ^ 1) {
				match = 1;
				min = sptr->ppid;
				sptr->flags |= 1;
			}
		}

		sptr = sptr->next;
	}

	return match == 1 ? min : -1;
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


void draw(Status *status)
{
	Status *sptr = rewindStatus(status);
	pid_t minppid = getMinPPid(status);

	while (minppid != -1) {

		// parent
		while (sptr) {

			if (sptr->pid == minppid) {
				printf(
					"%s %s [PID %d] [PPID %d]%s\n",
					ANSI_COLOR_RED,
					sptr->comm,
					sptr->pid,
					sptr->ppid,
					ANSI_COLOR_RESET
				);
				break;
			}

			sptr = sptr->next;

		}

		sptr = rewindStatus(status);

		// children
		if (minppid != 0) {
			while (sptr) {
				if (sptr->ppid == minppid && sptr->pid != minppid) {
					printf(
						"\t[%s%-20s%sPID: %s%-10d%sPPID: %s%d%s]\n",
						ANSI_COLOR_CYAN,
						sptr->comm,
						ANSI_COLOR_RESET,
						ANSI_COLOR_YELLOW,
						sptr->pid,
						ANSI_COLOR_RESET,
						ANSI_COLOR_GREEN,
						sptr->ppid,
						ANSI_COLOR_RESET
					);
				}

				sptr = sptr->next;
			}

			sptr = rewindStatus(status);

			printf("\n\n");
		}

		minppid = getMinPPid(status);
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
