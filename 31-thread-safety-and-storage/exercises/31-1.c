#include <pthread.h>
#include "../../lib/tlpi_hdr.h"


#define ONCE_CONTROL_INIT { 0, PTHREAD_MUTEX_INITIALIZER }


typedef struct once_control_t {

	int once;
	pthread_mutex_t lock;

} once_control_t;


once_control_t CONTROL = ONCE_CONTROL_INIT;


int one_time_init(once_control_t *control, void (*init)(void))
{
	int s;

	s = pthread_mutex_lock(&control->lock);
	if (s != 0) {
		return s;
	}

	if (!control->once) {
		init();
		control->once = 1;
	}

	s = pthread_mutex_unlock(&control->lock);
	if (s != 0) {
		return s;
	}

	return 0;
}


void initialization(void)
{
	printf("I am now initialized\n");
}


int main(int argc, char *argv[])
{
	// should only output once
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);
	one_time_init(&CONTROL, initialization);

	exit(EXIT_SUCCESS);
}
