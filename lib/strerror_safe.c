#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "tlpi_hdr.h"


static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;

#define MAX_ERROR_LEN 256


static void destructor(void *buf)
{
	free(buf);
}


static void createKey(void)
{
	int s;

	// allocate a unique thread_specific data ket and 
	// save the address of thre destructor for threas-specific data buffers
	s = pthread_key_create(&strerrorKey, destructor);
	if (s != 0) {
		errExitEN(s, "pthread_key_create");
	}
}


// sorta complicated
char *strerror_SAFE(int err)
{
	int s;
	char *buf;

	// make first caller create a key for this function
	s = pthread_once(&once, createKey);
	if (s != 0) {
		errExitEN(s, "pthread_once");
	}

	buf = pthread_getspecific(strerrorKey);
	// no block allocated for this thread yet
	if (buf == NULL) {
		buf = malloc(MAX_ERROR_LEN);
		if (buf == NULL) {
			errExit("malloc");
		}

		s = pthread_setspecific(strerrorKey, buf);
		if (s != 0) {
			errExitEN(s, "pthread_setspecific");
		}
	}

	if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
		snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
	} else {
		strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
		buf[MAX_ERROR_LEN - 1] = '\0';
	}

	return buf;
}
