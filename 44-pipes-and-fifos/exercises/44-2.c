#include <sys/wait.h>
#include <stdio.h>
#include "../../lib/tlpi_hdr.h"

#define MAX_COMMAND_LEN 2048

struct StreamNode {
    FILE *stream;
    int fd;
    pid_t pid;
    struct StreamNode *next;
    struct StreamNode *prev;
};

struct StreamList {
    int count;
    struct StreamNode *base;
};

static struct StreamList POPEN_STREAMS = { 0, NULL };


void add_popen_node(pid_t childPid, FILE *stream)
{
    struct StreamNode *np;
    struct StreamNode *node;

    node = calloc(1, sizeof(struct StreamNode));
    node->pid = childPid;
    node->stream = stream;

    if (POPEN_STREAMS.base == NULL) {
        POPEN_STREAMS.base = node;
    } else {
        np = POPEN_STREAMS.base;
        while (np->next != NULL) {
            np = np->next;
        }
        np->next = node;
        node->prev = np;
    }

    POPEN_STREAMS.count++;
}


int destroy_popen_node(struct StreamNode *node)
{
    int status;
    pid_t pid;

    if (node == NULL) {
        return -1;
    }

    pid = node->pid;

    // tricky one... don't forget to flush the stdio buffer!
    fflush(node->stream);

    // close() flushes the buffer, fclose() seems to not do so? In some cases?
    if (fclose(node->stream) != 0) {
        return -1;
    }

    if (node == POPEN_STREAMS.base) {
        if (node->next != NULL) {
            POPEN_STREAMS.base = node->next;
            node->next->prev = NULL;

        } else {
            POPEN_STREAMS.base = NULL;
        }

    } else {
        if (node->prev != NULL) {
            node->prev->next = node->next;
        }

        if (node->next != NULL) {
            node->next->prev = node->prev;
        }
    }

    free(node);

    POPEN_STREAMS.count--;

    if (waitpid(pid, &status, 0) == -1) {
        return -1;
    }

    return status;
}


int pclose2(FILE *stream)
{
    struct StreamNode *node;

    if (stream == NULL) {
        return -1;
    }

    node = POPEN_STREAMS.base;

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
    int read_or_write;
    pid_t childPid;
    FILE *stream;
    struct StreamNode *np;

    if (type == NULL || command == NULL || *type == '\0' || *command == '\0') {
        errno = EINVAL;
        return NULL;
    }

    if (pipe(fds) == -1) {
        return NULL;
    }

    // want to read the shell's output from the pipe
    if (strcmp(type, "r") == 0) {
        read_or_write = 0;

    // want to give the shell input from the pipe
    } else if (strcmp(type, "w") == 0) {
        read_or_write = 1;

    } else {
        errno = EINVAL;
        return NULL;
    }

    switch (childPid = fork()) {

        case -1:
            return NULL;

        case 0:
            // connect write end of the pipe to stdout of the shell process
            if (read_or_write == 0) {
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

            np = POPEN_STREAMS.base;
            while (np != NULL) {
                fclose(np->stream);
                np = np->next;
            }

            execl("/bin/sh", "sh", "-c", command, (char *)NULL);
            _exit(EXIT_FAILURE);

        // parent falls through
        default:
            break;
    }

    if (read_or_write == 0) {
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

    add_popen_node(childPid, stream);

    return stream;
}


int main(int argc, char *argv[])
{
    FILE *stream;
    FILE *stream2;
    FILE *stream3;
    FILE *stream4;
    FILE *stream5;

    char *mode;
    char data[1024];

    memset(data, 0, 1024);

    if (argc < 2) {
        usageErr("%s command [data]\n", argv[0]);
    }

    mode = argc == 3 ? "w" : "r";

    stream = popen2(argv[1], mode);
    stream2 = popen2(argv[1], mode);
    stream3 = popen2(argv[1], mode);
    stream4 = popen2(argv[1], mode);
    stream5 = popen2(argv[1], mode);

    if (stream == NULL) {
        errExit("popen2");
    }

    if (argc == 3) {

        fwrite(argv[2], strlen(argv[2]), 1, stream);
        fwrite(argv[2], strlen(argv[2]), 1, stream4);
        fwrite(argv[2], strlen(argv[2]), 1, stream2);
        fwrite(argv[2], strlen(argv[2]), 1, stream5);
        fwrite(argv[2], strlen(argv[2]), 1, stream3);

        pclose2(stream);
        pclose2(stream4);
        pclose2(stream2);
        pclose2(stream5);
        pclose2(stream3);

    } else {
        fread(data, 1024, 1, stream);
        printf("%s", data);
        fread(data, 1024, 1, stream2);
        printf("%s", data);
        fread(data, 1024, 1, stream3);
        printf("%s", data);
        fread(data, 1024, 1, stream4);
        printf("%s", data);
        fread(data, 1024, 1, stream5);
        printf("%s", data);

        pclose2(stream);
        pclose2(stream2);
        pclose2(stream3);
        pclose2(stream4);
        pclose2(stream5);
    }

    exit(EXIT_SUCCESS);
}
