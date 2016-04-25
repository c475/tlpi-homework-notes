#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../lib/tlpi_hdr.h"

#define BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
	int output_file = -1;
	char buffer[BUFFER_SIZE] = {0};

	ssize_t num_read;
	ssize_t num_written;

	int open_flags = O_CREAT | O_WRONLY;
	int status_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	int a_flag = getopt(argc, argv, "a");

	open_flags |= a_flag != -1 ? O_APPEND : O_TRUNC;

	if (argc > 1) {
		output_file = open(argv[argc-1], open_flags, status_flags);

		if (output_file == -1) {
			errExit("open() on output file");
		}
	}

	while ((num_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {

		if (num_read == -1) {
			errExit("read()");
		}

		if (output_file != -1) {

			num_written = write(output_file, buffer, num_read);

			if (num_written == -1 || num_written != num_read) {
				errExit("here!");
			}
		}

		num_written = write(STDOUT_FILENO, buffer, num_read);

		if (num_written == -1 || num_written != num_read) {
			errExit("here2!");
		}

		memset(buffer, 0, num_read);
	}

	if (output_file != -1) {
		if (close(output_file) == -1) {
			errExit("closing output file");
		}
	}
	
	exit(EXIT_SUCCESS);
}

