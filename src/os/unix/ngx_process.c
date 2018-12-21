#include <ngx_config.h>
//#include <ngx_process.h> // TODO 为何这里加上这行代码 ngx_process.h中的ngx_socket_t类型就无法识别？？？？
#include <ngx_core.h>
#include <ngx_channel.h>

int  ngx_argc;
char **ngx_argv;
char **ngx_os_argv;

/**
 * ngx_process_slot     进程的slot
 * ngx_socket_t         进程的管道
 * ngx_last_process     最后一个子进程的索引
 * ngx_processes        全局进程表 包含所有的存活的子进程
 */
ngx_int_t     ngx_process_slot;
ngx_socket_t  ngx_channel;
ngx_int_t     ngx_last_process;
ngx_process_t ngx_processes[NGX_MAX_PROCESSES];


/**
 * nginx在这里fork子进程
 *
 * @param cycle     全局配置
 * @param proc      子进程需要执行的函数
 * @param data      proc的参数
 * @param name      要创建的子进程的名字
 * @param respawn
 * @return
 */
ngx_pid_t
ngx_spawn_process(ngx_spawn_proc_pt proc, void *data,
                  char *name, ngx_int_t respawn) {
    u_long    on;
    ngx_pid_t pid;
    /* slot */
    ngx_int_t s;

    /**
     * 1 首先找到要fork出来的子进程要放在进程表ngx_processes的哪个位置 用s记录(slot)
     */
    if (respawn >= 0) { /* 如果传进来的类型大于0 就是确定这个进程已经退出了 可以直接确定位置 */
        s = respawn;

    } else {
        for (s = 0; s < ngx_last_process; s++) {
            if (ngx_processes[s].pid == -1) {
                break;
            }
        }

        if (s == NGX_MAX_PROCESSES) { /* 最大进程数 报错 */
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, 0,
//                          "no more than %d processes can be spawned",
//                          NGX_MAX_PROCESSES);
            return NGX_INVALID_PID;
        }
    }

    // 如果类型为NGX_PROCESS_DETACHED,则说明是热代码替换(热代码替换也是通过这个函数进行处理的),因此不需要新建socketpair。
    if (respawn != NGX_PROCESS_DETACHED) {

        /* Solaris 9 still has no AF_LOCAL */

        /**
         * 2 建立socketpair 用于进程间双向通讯
         */
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, ngx_processes[s].channel) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "socketpair() failed while spawning \"%s\"", name);
            return NGX_INVALID_PID;
        }

//        ngx_log_debug2(NGX_LOG_DEBUG_CORE, cycle->log, 0,
//                       "channel %d:%d",
//                       ngx_processes[s].channel[0],
//                       ngx_processes[s].channel[1]);
        // 设置非阻塞模式
        if (ngx_nonblocking(ngx_processes[s].channel[0]) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          ngx_nonblocking_n
//            " failed while spawning \"%s\"",
//                    name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;
        }

        if (ngx_nonblocking(ngx_processes[s].channel[1]) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          ngx_nonblocking_n
//            " failed while spawning \"%s\"",
//                    name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;
        }

        // 打开异步模式
        on = 1;
        if (ioctl(ngx_processes[s].channel[0], FIOASYNC, &on) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "ioctl(FIOASYNC) failed while spawning \"%s\"", name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;
        }

        // 设置异步io的所有者
        if (fcntl(ngx_processes[s].channel[0], F_SETOWN, ngx_pid) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "fcntl(F_SETOWN) failed while spawning \"%s\"", name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;
        }

        // 当exec后关闭句柄
        if (fcntl(ngx_processes[s].channel[0], F_SETFD, FD_CLOEXEC) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "fcntl(FD_CLOEXEC) failed while spawning \"%s\"",
//                          name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;
        }

        if (fcntl(ngx_processes[s].channel[1], F_SETFD, FD_CLOEXEC) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "fcntl(FD_CLOEXEC) failed while spawning \"%s\"",
//                          name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;
        }

        // 设置当前的子进程的句柄
        ngx_channel = ngx_processes[s].channel[1];

    } else { // respawn == NGX_PROCESS_DETACHED
        ngx_processes[s].channel[0] = -1;
        ngx_processes[s].channel[1] = -1;
    }

    /**
    * 3 fork子进程 在子进程中执行传入的proc函数
    */
    ngx_process_slot = s; // 设置进程在进程表中的slot。


    pid = fork();

    switch (pid) {

        case -1:
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "fork() failed while spawning \"%s\"", name);
//            ngx_close_channel(ngx_processes[s].channel, cycle->log);
            ngx_close_channel(ngx_processes[s].channel);
            return NGX_INVALID_PID;

        case 0:
            // 子进程 执行传递进来的子进程函数
            ngx_pid = ngx_getpid();
//            proc(cycle, data);
            proc(data);
            break;

        default:
            break;
    }

//    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "start %s %P", name, pid);

    ngx_processes[s].pid    = pid;
    ngx_processes[s].exited = 0;

    // 如果大于0,则说明我们确定了重启的子进程,因此下面的初始化就用已死的子进程的就够了。
    if (respawn >= 0) {
        return pid;
    }

    ngx_processes[s].proc    = proc;
    ngx_processes[s].data    = data;
    ngx_processes[s].name    = name;
    ngx_processes[s].exiting = 0;

    //设置相关状态。
    switch (respawn) {

        case NGX_PROCESS_RESPAWN:
            ngx_processes[s].respawn      = 1;
            ngx_processes[s].just_respawn = 0;
            ngx_processes[s].detached     = 0;
            break;

        case NGX_PROCESS_JUST_RESPAWN:
            ngx_processes[s].respawn      = 1;
            ngx_processes[s].just_respawn = 1;
            ngx_processes[s].detached     = 0;
            break;

        case NGX_PROCESS_DETACHED:
            ngx_processes[s].respawn      = 0;
            ngx_processes[s].just_respawn = 0;
            ngx_processes[s].detached     = 1;
            break;
    }

    if (s == ngx_last_process) {
        ngx_last_process++;
    }

    return pid;
}
