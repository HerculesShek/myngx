#include <stdlib.h>
#include <stdio.h>
typedef pid_t ngx_pid_t;
typedef int ngx_socket_t;
typedef void (*ngx_spawn_proc_pt)(void *data);
typedef int ngx_int_t;

#define NGX_MAX_PROCESSES 1024

typedef struct {
    ngx_pid_t pid;            /* 进程id */
    int status;         /* 进程退出状态 */
    ngx_socket_t channel[2];     /* 进程socketpair创建的句柄 */

    ngx_spawn_proc_pt proc;           /* 进程要执行的函数 */
    void *data;           /* 进程所执行函数proc的参数 */
    char *name;           /* 进程名字 */

    /* 进程的几个状态 bitfield 默认值是0 */
    unsigned respawn:1;
    unsigned just_respawn:1;
    unsigned detached:1;
    unsigned exiting:1;
    unsigned exited:1;
} ngx_process_t;

ngx_int_t ngx_last_process;
ngx_process_t ngx_processes[NGX_MAX_PROCESSES];

int main() {
    printf("ngx_last_process:%i\n", ngx_last_process);
    for (int i = 0; i < 10; ++i) {
        ngx_process_t np = ngx_processes[i];
        printf("index:%i, name:%s, pid:%i\n", i, np.name, np.pid);
    }
    return 0;
}