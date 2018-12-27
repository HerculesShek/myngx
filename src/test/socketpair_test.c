
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// https://stackoverflow.com/questions/11461106/socketpair-in-c-unix

// socketpair creates an anonymous pair of sockets, usually unix/local sockets,
// which are only useful for communication between a parent and child process or
// in other cases where the processes that need to use them can inherit the file
// descriptors from a common ancestor.

void child(int socket);
void parent(int socket);

static const int parentsocket = 0;
static const int childsocket  = 1;

int main() {
    int   channel[2];
    pid_t pid;

    /* 1. call socketpair ... */
    printf("before socketpair:\n\tchannel[0]:%i, channel[1]:%i\n", channel[0], channel[1]); // channel[0]:-349112584, channel[1]:32766
    if (socketpair(PF_LOCAL, SOCK_STREAM, 0, channel) == -1) { // channel[0]:3, channel[1]:4
        printf("create unnamed socket pair failed:%s\n", strerror(errno));
        return 1;
    }
    printf("after socketpair:\n\tchannel[0]:%i, channel[1]:%i\n\n", channel[0], channel[1]);

    /* 2. call fork ... */
    pid = fork();
    if (pid == -1) { ;
    } else if (pid == 0) {
        printf("Child: channel[0]:%i, channel[1]:%i, addr: %p\n", channel[0], channel[1], &channel);
        close(channel[parentsocket]); /* Close the parent file descriptor */
        child(channel[childsocket]);
    } else {
        printf("Parent: channel[0]:%i, channel[1]:%i, addr: %p\n", channel[0], channel[1], &channel);
        close(channel[childsocket]); /* Close the child file descriptor */
        parent(channel[parentsocket]);
    }

    return 0;
}

void child(int socket) {
    sleep(2);
    const char hello[] = "hello parent, I am child";
    write(socket, hello, sizeof(hello)); /* NB. this includes nul */
    /* go forth and do childish things with this end of the pipe */

    char    buf[1024];
    ssize_t n          = read(socket, buf, sizeof(buf));
    printf("child received '%.*s'\n", (int) n, buf);
}

void parent(int socket) {
    /* do parental things with this end, like reading the child's message */
    char    buf[1024];
    ssize_t n = read(socket, buf, sizeof(buf));
    printf("parent received '%.*s'\n", (int) n, buf);

    sleep(3);
    const char res[] = "hello child, I received your message!";
    write(socket, res, sizeof(res)); /* NB. this includes nul */
    /* go forth and do childish things with this end of the pipe */

}