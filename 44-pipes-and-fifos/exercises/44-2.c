#include <sys/wait.h>
#include <stdio.h>
#include "../../lib/tlpi_hdr.h"

#define MAX_COMMAND_LEN 2048

struct StreamNode {
    FILE *stream;
    pid_t pid;
    struct StreamNode *next;
    struct StreamNode *prev;
};

struct StreamList {
    int count;
    struct StreamNode *base;
};

static struct StreamList *POPEN_STREAMS = NULL;


int destroy_popen_node(struct StreamNode *node)
{
    int status;
    pid_t pid;

    if (node == NULL) {
        return -1;
    }

    pid = node->pid;

    if (fclose(node->stream) != 0) {
        return -1;
    }

    if (waitpid(pid, &status, 0) == -1) {
        return -1;
    }

    node->prev->next = node->next;
    free(node);

    return status;
}


int pclose2(FILE *stream)
{
    struct StreamNode *node;

    if (POPEN_STREAMS == NULL) {
        return -1;
    }

    node = POPEN_STREAMS->base;

    while (node != NULL) {
        if (node->stream == stream) {
            break;
        }

        node = node->next;
    }

    return destroy_popen_node(node);
}

FILE *popen2(const char *command, const char *type)
{
    int fds[2];
    int type;
    FILE *stream;

    if (!type || !command) {
        errno = EINVAL;
        return NULL;
    }

    if (pipe(fds) == -1) {
        return NULL;
    }

    // want to read the shell's output from the pipe
    if (strcmp(type, "r") == 0) {
        type = 0;

    // want to give the shell input from the pipe
    } else if (strcmp(type, "w") == 0) {
        type = 1;

    } else {
        errno = EINVAL;
        return NULL;
    }

    switch (fork()) {

        case -1:
            return NULL;

        case 0:
            // connect write end of the pipe to stdout of the shell process
            if (type == 0) {
                if (close(fds[0]) == -1) {
                    _exit(EXIT_FAILURE);
                }

                if (dup2(fds[1], STDOUT_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }

            // connect read end of the pipe to stdin of the shell process
            } else {
                if (close(fds[1]) == -1) {
                    _exit(EXIT_FAILURE);
                }

                if (dup2(fds[0], STDIN_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
            }

            execl("/bin/sh", "sh", "-c", command, (char *)NULL);
            _exit(EXIT_FAILURE);

        // parent falls through
        default:
            break;
    }

    if (type == 0) {
        if (close(fds[1]) == -1) {
            close(fds[0]);
            return NULL;
        }

        stream = fdopen(fds[0], "r");

    } else {
        if (close(fds[0]) == -1) {
            close(fds[1]);
            return NULL;
        }

        stream = fdopen(fds[1], "w");
    }

    return stream;
}


int main(int argc, char *argv[])
{
    FILE *stream;
    char *mode;

    if (argc != 2 || argc != 3) {
        usageErr("%s command [data]\n", argv[0]);
    }

    mode = argc == 3 ? "w" : "r";

    stream = popen2(argv[1], mode);
    if (stream == NULL) {
        errExit("popen2");
    }


}