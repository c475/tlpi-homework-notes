#include <fcntl.h>
#include "../../lib/tlpi_hdr.h"

#define BUFFER 1024

// attr       clear_refs       cpuset   fd       limits     mem         net        oom_score      projid_map  sessionid  stat     task
// autogroup  cmdline          cwd      fdinfo   loginuid   mountinfo   ns         oom_score_adj  root        setgroups  statm    timers
// auxv       comm             environ  gid_map  map_files  mounts      numa_maps  pagemap        sched       smaps      status   uid_map
// cgroup     coredump_filter  exe      io       maps       mountstats  oom_adj    personality    schedstat   stack      syscall  wchan

int main(int argc, char *argv[])
{
	char rbuffer[BUFFER] = {0};
	int num_read = 0;
	int fd;

	while ((fd = open("/proc/self/maps", O_RDONLY)) != -1) {
		num_read = read(fd, rbuffer, BUFFER);

		if (num_read == -1) {
			fprintf(stderr, "read");
			exit(EXIT_FAILURE);
		}

		printf("%s\r", rbuffer);
		memset(rbuffer, 0, num_read);
		close(fd);
	}

	exit(EXIT_SUCCESS);
}
