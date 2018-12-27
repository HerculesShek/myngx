#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef pid_t ngx_pid_t;
typedef int   ngx_socket_t;
typedef void (*ngx_spawn_proc_pt)(void *data);
typedef int ngx_int_t;

#define NGX_MAX_PROCESSES 1024

typedef struct {
    ngx_pid_t    pid;            /* 进程id */
    int          status;         /* 进程退出状态 */
    ngx_socket_t channel[2];     /* 进程socketpair创建的句柄 */

    ngx_spawn_proc_pt proc;           /* 进程要执行的函数 */
    void              *data;           /* 进程所执行函数proc的参数 */
    char              *name;           /* 进程名字 */

    /* 进程的几个状态 bitfield 默认值是0 */
    unsigned respawn:1;
    unsigned just_respawn:1;
    unsigned detached:1;
    unsigned exiting:1;
    unsigned exited:1;
}           ngx_process_t;

ngx_int_t     ngx_last_process;
ngx_process_t ngx_processes[NGX_MAX_PROCESSES];

void print_procs();

/**
 * 这里说明父进程的变量在fork之后会复制到子进程中
 * 但是是一个副本 指针的话就是一样的了
 */

int main() {
    int   com    = 100;
    int   arr[2] = {10, 20};
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        printf("fork failed...");
        return 1;
    } else if (pid == 0) { // child process
        sleep(2);
        print_procs();
    } else { // parent process
        ngx_processes[0].pid = 990;
        ngx_processes[0].channel[0] = 3;
        ngx_processes[0].channel[1] = 4;
        sleep(3);
    }

    return 0;
}

void print_procs() {
    for (int i = 0; i < 3; ++i) {
        ngx_process_t np = ngx_processes[i];
        printf("\tindex:%i, name:%s, pid:%i, channel[0]:%i, channel[1]:%i\n",
               i,
               np.name,
               np.pid,
               np.channel[0],
               np.channel[1]);
    }
}