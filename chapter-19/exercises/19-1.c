#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <sys/inotify.h>
#include <ftw.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include "../../lib/tlpi_hdr.h"


#define MAX_WATCH 1024


typedef struct WatchNode {
	int fd;
	char *name;
	struct WatchNode *prev;
	struct WatchNode *next;
} WatchNode;


typedef struct WatchList {
	int len;
	WatchNode *first;
	WatchNode *last;
} WatchList;


typedef struct Book {
	int fd;
	uint8_t cookie;
} Book;



#define WATCH_FLAGS (IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_DELETE_SELF)
#define BUFFER_SIZE (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define LOG_LINE 4098


static WatchList *TheList = NULL;
static Book InotifyBook;


WatchList *create_list(void)
{
	return calloc(1, sizeof(WatchList));
}


WatchNode *add_watch(WatchList *list, int fd, const char *filename)
{
	WatchNode *node = NULL;

	if (list != NULL) {

		node = calloc(1, sizeof(WatchNode));
		if (node == NULL) {
			return NULL;
		}

		node->fd = fd;
		node->name = calloc(1, strlen(filename) + 1);
		strncpy(node->name, filename, strlen(filename));

		if (list->last == NULL) {
			list->first = node;
			list->last = node;
		} else {
			list->last->next = node;
			node->prev = list->last;
			list->last = node;
		}

		list->len++;
	}

	return node;
}


int rm_watch(WatchList *list, int fd)
{
	int wd;
	WatchNode *node;

	if (list == NULL) {
		return -1;
	}

	node = list->first;

	while (node != NULL) {

		if (node->fd == fd) {

			if (node == list->first && node == list->last) {
				list->first = NULL;
				list->last = NULL;
				break;
			} else if (node == list->first) {
				list->first = node->next;
				list->first->prev = NULL;
				break;
			} else if (node == list->last) {
				list->last = node->prev;
				list->last->next = NULL;
				break;
			} else {
				node->prev->next = node->next;
				node->next->prev = node->prev;
				break;
			}

		}

		node = node->next;
	}

	if (node == NULL) {
		return -1;
	}

	inotify_rm_watch(InotifyBook.fd, fd);

	wd = node->fd;
	list->len--;

	free(node->name);
	free(node);

	return wd;
}


int is_watched(WatchList *list, int fd)
{
	WatchNode *tmp;

	if (list == NULL || list->first == NULL) {
		return -1;
	}

	tmp = list->first;

	while (tmp != NULL) {
		if (tmp->fd == fd) {
			return 1;
		}

		tmp = tmp->next;
	}

	return 0;
}


int func(const char *pathname, const struct stat *sb, int type, struct FTW *ftw)
{
	int fd;

	fd = inotify_add_watch(InotifyBook.fd, pathname, WATCH_FLAGS);
	if (fd == -1) {
		return -1;
	}

	if (!is_watched(TheList, fd)) {
		if (add_watch(TheList, fd, pathname) == NULL) {
			return -1;
		}
	}

	return 0;
}


int setup(int argc, char **argv)
{
	InotifyBook.fd = inotify_init();
	if (InotifyBook.fd == -1) {
		return -1;
	}

	return 0;
}


int sync_directory(const char *dirname)
{
	if (nftw(dirname, func, MAX_WATCH, FTW_PHYS) == -1) {
		return -1;
	}

	return 0;
}


char *gettime(void)
{
	time_t t = time(NULL);
	char *strt = ctime(&t);
	strt[24] = '\0';
	return strt;
}


void write_event(int fd, const char *str, ...)
{
	char buf[LOG_LINE];

	va_list list;
	va_start(list, str);

	memset(buf, 0, LOG_LINE);

	if (str == NULL || *str == '\0') {
		return;
	}

	vsnprintf(buf, LOG_LINE, str, list);

	write(fd, buf, strlen(buf));

	va_end(list);
}


// close enough
int handle_event(const char *dirname, int logfile, struct inotify_event *event)
{
	if (event->mask & IN_IGNORED) {
		return 0;
	}

	if (access(".", F_OK) == -1) {
		return -1;
	}

	if (event->mask & IN_DELETE) {
		inotify_rm_watch(InotifyBook.fd, event->wd);
		rm_watch(TheList, event->wd);

		if (event->mask & IN_ISDIR) {
			write_event(logfile, "[%s] deleted directory: %s\n", gettime(), event->name);
		} else {
			write_event(logfile, "[%s] deleted file: %s\n", gettime(), event->name);
		}

	} else if (event->mask & IN_CREATE) {
		if (event->mask & IN_ISDIR) {
			write_event(logfile, "[%s] created directory: %s\n", gettime(), event->name);
		} else {
			write_event(logfile, "[%s] created file: %s\n", gettime(), event->name);
		}

	} else if (event->mask & IN_MOVED_FROM) {
		InotifyBook.cookie = event->cookie;
		write_event(logfile, "[%s] directory moved from watch zone: %s\n", gettime(), event->name);

	} else if (event->mask & IN_MOVED_TO) {
		write_event(logfile, "[%s] directory moved into watched directory: %s\n", gettime(), event->name);
	}

	sync_directory(dirname);

	return 0;
}


int main(int argc, char *argv[])
{
	int logfile;
	int running;
	ssize_t num_read;
	char buff[BUFFER_SIZE] __attribute__((aligned(8)));
	char *p;
	struct inotify_event *event;

	memset(&InotifyBook, 0, sizeof(Book));

	running = 1;

	logfile = open("logfile.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
	if (logfile == -1) {
		errExit("open()");
	}

	TheList = create_list();
	if (TheList == NULL) {
		errExit("create_list()");
	}

	if (setup(argc,	argv) == -1) {
		errExit("setup()");
	}

	if (sync_directory(argv[1]) == -1) {
		errExit("nftw()");
	}

	while (running) {

		num_read = read(InotifyBook.fd, buff, BUFFER_SIZE);

		if (num_read == 0) {
			fatal("0 bytes read from inotify descriptor\n");
		}

		if (num_read == -1) {
			errExit("read()");
		}

		for (p = buff; p < buff + num_read;) {
			event = (struct inotify_event *)p;

			if (handle_event(argv[1], logfile, event) == -1) {
				running = 0;
				break;
			}
			
			p += sizeof(struct inotify_event) + event->len;
		}
	}

	exit(EXIT_SUCCESS);
}
