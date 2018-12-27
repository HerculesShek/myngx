#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int com    = 100;
int arr[2] = {10, 20};
/**
 * 这里说明父进程的变量在fork之后会复制到子进程中
 * 但是是一个副本 指针的话就是一样的了
 */
int main() {
    pid_t pid;
    pid = fork();
    if (pid == -1) { ;
    } else if (pid == 0) {
        sleep(2);
        printf("Child: com is %i\n", com);
        printf("Child: arr[0] is %i\n", arr[0]);
        com += 12;
    } else {
        com += 13;
        printf("Parent: com is %i\n", com);
        arr[0] += 10;
        sleep(3);
        printf("Parent: com is %i\n", com);
    }

    return 0;
}