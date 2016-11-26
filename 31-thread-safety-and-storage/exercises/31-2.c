#include <pthread.h>
#include <limits.h>
#include <libgen.h>
#include "../../lib/tlpi_hdr.h"


static pthread_once_t dirname_once = PTHREAD_ONCE_INIT;
static pthread_once_t basename_once = PTHREAD_ONCE_INIT;
static pthread_key_t dirname_key;
static pthread_key_t basename_key;


static void key_storage_destructor(void *buf)
{
	free(buf);
}


static void create_dirname_key(void)
{
	int s;

	s = pthread_key_create(&dirname_key, key_storage_destructor);
	if (s != 0) {
		errExitEN(s, "pthread_key_create");
	}
}


static void create_basename_key(void)
{
	int s;

	s = pthread_key_create(&basename_key, key_storage_destructor);
	if (s != 0) {
		errExitEN(s, "pthread_key_create");
	}
}


char *dirname2(char *path)
{
	int s;
	char *buffer;
	char *temp;

	s = pthread_once(&dirname_once, create_dirname_key);
	if (s != 0) {
		errExitEN(s, "pthread_once");
	}

	buffer = pthread_getspecific(dirname_key);
	if (buffer == NULL) {
		buffer = calloc(1, PATH_MAX + 1);
		if (buffer == NULL) {
			errExit("calloc");
		}

		s = pthread_setspecific(dirname_key, buffer);
		if (s != 0) {
			errExitEN(s, "pthread_set_specific");
		}
	}

	temp = buffer;
	while (*temp) *temp++ = '\0';

	if (strchr(path, '/') == NULL) {
		*buffer = '.';
		return buffer;
	}

	strncpy(buffer, path, PATH_MAX);

	temp = strrchr(buffer, '/');
	if (*(temp + 1) == '\0' && temp != buffer) {
		while (*temp == '/') temp--;
		while (*temp != '/') temp--;
		while (*temp == '/' && temp != buffer) temp--;

	} else if (temp != buffer) {
		*temp = '\0';
	}

	if (temp != buffer) {
		*(temp + 1) = '\0';
	}

	return buffer;
}


char *basename2(char *path)
{
	int s;
	char *buffer;
	char *temp;
	char *temp2;

	s = pthread_once(&basename_once, create_basename_key);
	if (s != 0) {
		errExitEN(s, "pthread_once");
	}

	buffer = pthread_getspecific(basename_key);
	if (buffer == NULL) {
		buffer = calloc(1, PATH_MAX + 1);
		if (buffer == NULL) {
			errExit("calloc");
		}

		s = pthread_setspecific(basename_key, buffer);
		if (s != 0) {
			errExitEN(s, "pthread_set_specific");
		}
	}

	temp = buffer;
	while (*temp) *temp++ = '\0';

	if (strchr(path, '/') == NULL || strcmp(path, "/") == 0) {
		strncpy(buffer, path, PATH_MAX);
		return buffer;
	}

	temp = strrchr(path, '/');
	temp2 = buffer;
	if (*(temp + 1) == '\0') {
		while (*temp == '/') temp--;
		while (*temp != '/' && temp != path) temp--;
		if (*temp == '/') {
			temp++;
		}
	} else {
		temp++;
	}

	while (*temp != '\0' && *temp != '/') *temp2++ = *temp++;

	return buffer;
}


void *threadFunc(void *arg)
{
	char lol[] = "/var/////log/////";
	printf("dirname /var/log/cool.txt: %s\n", dirname2("/var/log/cool.txt"));
	printf("dirname /var/log/cool.txt/: %s\n", dirname2("/var/log/cool.txt/"));
	printf("*dirname /var/////log/////: %s\n", dirname2("/var/////log/////"));
	printf("*dirname /var/////log/////: %s\n", dirname2("/var/////log/////"));
	printf("*dirname test: %s\n", dirname2("test"));
	printf("dirname /: %s\n", dirname2("/"));

	printf("SPECIAL DIRNAME /var/log/////: %s\n", dirname(lol));


	printf("basename2 /var/log/cool.txt: %s\n", basename2("/var/log/cool.txt"));
	printf("basename2 /var/log/cool.txt/: %s\n", basename2("/var/log/cool.txt/"));
	printf("*basename2 /var/////log/////: %s\n", basename2("/var/////log/////"));
	printf("*basename2 /var/////log/////: %s\n", basename2("/var/////log/////"));
	printf("*basename2 test: %s\n", basename2("test"));
	printf("basename2 /: %s\n", basename2("/"));

	return NULL;
}


int main(int argc, char *argv[])
{
	int s;
	pthread_t t1;

	s = pthread_create(&t1, NULL, threadFunc, (void *)NULL);
	if (s != 0) {
		errExitEN(s, "pthread_create");
	}

	s = pthread_join(t1, NULL);
	if (s != 0) {
		errExitEN(s, "pthread_join");
	}

	exit(EXIT_SUCCESS);
}
