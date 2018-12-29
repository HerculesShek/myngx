
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_channel.h>
//#include <time.h>
//#include <stdlib.h>

u_long        cpu_affinity;
static u_char master_process[] = "master process";


static void ngx_start_worker_processes(ngx_int_t n, ngx_int_t type);

static void ngx_start_garbage_collector(ngx_int_t type);


//static void ngx_signal_worker_processes(ngx_cycle_t *cycle, int signo);
//static ngx_uint_t ngx_reap_childs(ngx_cycle_t *cycle);
//static void ngx_master_process_exit(ngx_cycle_t *cycle);
static void ngx_worker_process_cycle(void *data);
static void ngx_worker_process_init(ngx_uint_t priority);
//static void ngx_worker_process_exit(ngx_cycle_t *cycle);
//static void ngx_channel_handler(ngx_event_t *ev);
static void print_sigset(const sigset_t *set, const char *name);


ngx_uint_t ngx_process;
ngx_pid_t  ngx_pid;
ngx_uint_t ngx_threaded;

sig_atomic_t ngx_reap;
sig_atomic_t ngx_sigio;
sig_atomic_t ngx_terminate;
sig_atomic_t ngx_quit;
sig_atomic_t ngx_debug_quit;
ngx_uint_t   ngx_exiting;
sig_atomic_t ngx_reconfigure;
sig_atomic_t ngx_reopen;

sig_atomic_t ngx_change_binary;
ngx_pid_t    ngx_new_binary;
ngx_uint_t   ngx_inherited;
ngx_uint_t   ngx_daemonized;

sig_atomic_t ngx_noaccept;
ngx_uint_t   ngx_noaccepting;
ngx_uint_t   ngx_restart;


void
ngx_master_process_cycle() {
    char             *title;
    u_char           *p;
    size_t           size;
    ngx_int_t        i;
    ngx_uint_t       n;
    sigset_t         set;
    struct itimerval itv;
    ngx_uint_t       live;
    ngx_msec_t       delay;
//    ngx_listening_t   *ls;
//    ngx_core_conf_t   *ccf;

    // 初始化信号集 并设置接受相关的信号
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGIO);
    sigaddset(&set, SIGINT);
    sigaddset(&set, ngx_signal_value(NGX_RECONFIGURE_SIGNAL)); // SIGHUP
    sigaddset(&set, ngx_signal_value(NGX_REOPEN_SIGNAL)); // SIGUSR1
    sigaddset(&set, ngx_signal_value(NGX_NOACCEPT_SIGNAL)); //SIGWINCH
    sigaddset(&set, ngx_signal_value(NGX_TERMINATE_SIGNAL)); //SIGTERM
    sigaddset(&set, ngx_signal_value(NGX_SHUTDOWN_SIGNAL)); //SIGQUIT
    sigaddset(&set, ngx_signal_value(NGX_CHANGEBIN_SIGNAL)); // SIGUSR2

//    printf("sizeof sigset_t is %lu\n", sizeof(sigset_t)); // mac是 4 centos7 x64 是128 看来 sizeof在不同系统上代表的意义也不同
//    print_sigset(&set, "set");

    // 把以上信号全部加入信号屏蔽字中！
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
//        ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                      "sigprocmask() failed");
        printf("sigprocmask failed...\n");
    }

//    sigemptyset(&set);
//    print_sigset(&set, "set");

    size = sizeof(master_process);
//    printf("size is %zu\n", size);

    for (i = 0; i < ngx_argc; i++) {
        size += ngx_strlen(ngx_argv[i]) + 1;
    }

//    title = ngx_palloc(cycle->pool, size);
//
//    p      = ngx_cpymem(title, master_process, sizeof(master_process) - 1);
//    for (i = 0; i < ngx_argc; i++) {
//        *p++ = ' ';
//        p = ngx_cpystrn(p, (u_char *) ngx_argv[i], size);
//    }

//    ngx_setproctitle(title);


//    ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx, ngx_core_module);

//    ngx_start_worker_processes(cycle, ccf->worker_processes,
//                               NGX_PROCESS_RESPAWN);
//    ngx_start_garbage_collector(cycle, NGX_PROCESS_RESPAWN);

//    printf("ngx_argc is %i\n", ngx_argc);

    ngx_start_worker_processes(4, NGX_PROCESS_RESPAWN);
    ngx_start_garbage_collector(NGX_PROCESS_RESPAWN);

    ngx_new_binary = 0;
    delay          = 0;
    live           = 1;

    // delay用来等待子进程退出的时间,由于我们接受到SIGINT信号后,
    // 我们需要先发送信号给子进程,而子进程的退出需要一定的时间,超时时如果子进程已退出,
    // 我们父进程就直接退出,否则发送sigkill信号给子进程(强制退出),然后再退出。
    for (;;) {
        if (delay) {
            delay *= 2;

//            ngx_log_debug1(NGX_LOG_DEBUG_EVENT, cycle->log, 0,
//                           "temination cycle: %d", delay);

            itv.it_interval.tv_sec  = 0;
            itv.it_interval.tv_usec = 0;
            itv.it_value.tv_sec     = delay / 1000;
            itv.it_value.tv_usec    = (delay % 1000) * 1000;
            // 设置定时器
            if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
//                ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                              "setitimer() failed");
            }
        }

//        ngx_log_debug0(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "sigsuspend");

        // 延时,等待定时器
        sigsuspend(&set);

//        ngx_time_update(0, 0);
//
//        ngx_log_debug0(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "wake up");

        // ngx_reap为1,说明有子进程已经退出
//        if (ngx_reap) {
//            ngx_reap = 0;
////            ngx_log_debug0(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "reap childs");
//
//            // 这个里面处理退出的子进程(有的worker异常退出,这时我们就需要重启这个worker
//            live = ngx_reap_childs(cycle);
//        }
//
//        // 如果没有存活的子进程,并且收到了ngx_terminate或者ngx_quit信号,则master退出。
//        if (!live && (ngx_terminate || ngx_quit)) {
//            ngx_master_process_exit(cycle);
//        }
//
//        // 收到了sigint信号
//        if (ngx_terminate) {
//            // 设置延时
//            if (delay == 0) {
//                delay = 50;
//            }
//
//            if (delay > 1000) {
//                // 如果超时,则强制杀死worker
//                ngx_signal_worker_processes(cycle, SIGKILL);
//            } else {
//                // 负责发送sigint给worker,让它退出
//                ngx_signal_worker_processes(cycle,
//                                            ngx_signal_value(NGX_TERMINATE_SIGNAL));
//            }
//
//            continue;
//        }
//
//        // 收到quit信号
//        if (ngx_quit) {
//            // 发送给worker quit信号
//            ngx_signal_worker_processes(cycle,
//                                        ngx_signal_value(NGX_SHUTDOWN_SIGNAL));
//
//            ls = cycle->listening.elts;
//            for (n = 0; n < cycle->listening.nelts; n++) {
//                if (ngx_close_socket(ls[n].fd) == -1) {
//                    ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_socket_errno,
//                                  ngx_close_socket_n " %V failed",
//                            &ls[n].addr_text);
//                }
//            }
//            cycle->listening.nelts = 0;
//
//            continue;
//        }
//
//        // 收到需要reconfig的信号
//        if (ngx_reconfigure) {
//            ngx_reconfigure = 0;
//
//            // 判断是否热代码替换后的新的代码还在运行中(也就是还没退出当前的master)。如果还在运行中,则不需要重新初始化config
//            if (ngx_new_binary) {
//                ngx_start_worker_processes(cycle, ccf->worker_processes,
//                                           NGX_PROCESS_RESPAWN);
//                ngx_start_garbage_collector(cycle, NGX_PROCESS_RESPAWN);
//                ngx_noaccepting = 0;
//
//                continue;
//            }
//
////            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "reconfiguring");
//
//            // 重新初始化config,并重新启动新的worker
//            cycle = ngx_init_cycle(cycle);
//            if (cycle == NULL) {
//                cycle = (ngx_cycle_t *) ngx_cycle;
//                continue;
//            }
//
//            ngx_cycle = cycle;
//            ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx,
//                                                   ngx_core_module);
//            ngx_start_worker_processes(cycle, ccf->worker_processes,
//                                       NGX_PROCESS_JUST_RESPAWN);
//            ngx_start_garbage_collector(cycle, NGX_PROCESS_JUST_RESPAWN);
//            live = 1;
//            ngx_signal_worker_processes(cycle,
//                                        ngx_signal_value(NGX_SHUTDOWN_SIGNAL));
//        }
//
//        // 这个标志没弄懂有什么意义。代码里面是当热代码替换后,如果ngx_noacceptig被设置了,则设置这个标志位(难道意思是热代码替换前要先停止当前的accept连接?)
//        if (ngx_restart) {
//            ngx_restart = 0;
//            ngx_start_worker_processes(cycle, ccf->worker_processes,
//                                       NGX_PROCESS_RESPAWN);
//            ngx_start_garbage_collector(cycle, NGX_PROCESS_RESPAWN);
//            live = 1;
//        }
//
//        // 重新打开log
//        if (ngx_reopen) {
//            ngx_reopen = 0;
////            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "reopening logs");
//            ngx_reopen_files(cycle, ccf->user);
//            ngx_signal_worker_processes(cycle,
//                                        ngx_signal_value(NGX_REOPEN_SIGNAL));
//        }
//
//        ///热代码替换
//        if (ngx_change_binary) {
//            ngx_change_binary = 0;
////            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "changing binary");
//            ///进行热代码替换,这里是调用execve来执行新的代码。
//            ngx_new_binary = ngx_exec_new_binary(cycle, ngx_argv);
//        }
//
//        ///接受到停止accept连接,其实也就是worker退出(有区别的是,这里master不需要退出).
//        if (ngx_noaccept) {
//            ngx_noaccept = 0;
//            ngx_noaccepting = 1;
//            ///给worker发送信号
//            ngx_signal_worker_processes(cycle,
//                                        ngx_signal_value(NGX_SHUTDOWN_SIGNAL));
//        }
    }
}


/**
 *
 * @param cycle
 * @param n
 * @param type
 */
static void ngx_start_worker_processes(ngx_int_t n, ngx_int_t type) {
    ngx_int_t     i, s;
    ngx_channel_t ch;

//    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "start worker processes");

    // 传递给其他子进程的命令
    ch.command = NGX_CMD_OPEN_CHANNEL;

    // 这里n,就是从配置文件中读取的,需要fork几个子进程。
    for (i = 0; i < n; i++) {

//        cpu_affinity = ngx_get_cpu_affinity(i);
        // fork子进程 worker process

        printf("pid[%i] will fork [%i]nd sub-process\n", getpid(), (i + 1));
        ngx_spawn_process(ngx_worker_process_cycle, NULL, "worker process", type);

        // 初始化channel,ngx_process_slot这个我们在上面的spawn函数中已经赋值完毕,就是当前子进程的位置。
        ch.pid  = ngx_processes[ngx_process_slot].pid;
        ch.slot = ngx_process_slot;
        ch.fd   = ngx_processes[ngx_process_slot].channel[0];

        ///遍历整个进程表
        for (s = 0; s < ngx_last_process; s++) {
            // 遇到非存活的进程就跳过
            if (s == ngx_process_slot
                || ngx_processes[s].pid == -1
                || ngx_processes[s].channel[0] == -1) {
                continue;
            }

//            ngx_log_debug6(NGX_LOG_DEBUG_CORE, cycle->log, 0,
//                           "pass channel s:%d pid:%P fd:%d to s:%i pid:%P fd:%d",
//                           ch.slot, ch.pid, ch.fd,
//                           s, ngx_processes[s].pid,
//                           ngx_processes[s].channel[0]);

            /* TODO: NGX_AGAIN */
            // 然后传递这个channel给其他子进程(主要是传递句柄)。
            printf("master is sending to %i cmd...\n", ngx_processes[s].pid);
            ngx_write_channel(ngx_processes[s].channel[0], &ch, sizeof(ngx_channel_t));
        }
    }
}


static void
ngx_worker_process_cycle(void *data) {

    ngx_worker_process_init(1);

//    ngx_setproctitle("worker process");


//    srand(time(NULL));   // Initialization, should only be called once.
    srand((unsigned int) ngx_pid);
    printf("\tpid[%i] is begin....\n", getpid());

    // todo will 工作进程必须是死循环 否则会创建多余进程
    for (;;) {
        static count = 0;
        int    r     = rand();
        int    ss    = r % 7;
        ss += 3;

        printf("\tpid[%i] is working on [%i] round(s) will sleep [%i] seconds...\n", ngx_pid, count, ss);
        count++;
//        sleep(ss);
        delay(ss);

        // todo will 父子间通讯
        ngx_channel_t ch;
        ngx_int_t     n;
        n = ngx_read_channel(ngx_processes[ngx_process_slot].channel[1], &ch, sizeof(ngx_channel_t));
        if (n == sizeof(ngx_channel_t)) {

        }
        if (count > 4) {
            printf("pid[%i] is exiting...\n", ngx_pid);
            exit(0);
        }
    }
//    for ( ;; ) {
//        if (ngx_exiting
//            && ngx_event_timer_rbtree.root == ngx_event_timer_rbtree.sentinel)
//        {
//            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exiting");
//
//            ngx_worker_process_exit(cycle);
//        }
//
//        ngx_log_debug0(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "worker cycle");
//
//        ngx_process_events_and_timers(cycle);
//
//        if (ngx_terminate) {
//            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exiting");
//
//            ngx_worker_process_exit(cycle);
//        }
//
//        if (ngx_quit) {
//            ngx_quit = 0;
//            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0,
//                          "gracefully shutting down");
//            ngx_setproctitle("worker process is shutting down");
//
//            if (!ngx_exiting) {
//                ngx_close_listening_sockets(cycle);
//                ngx_exiting = 1;
//            }
//        }
//
//        if (ngx_reopen) {
//            ngx_reopen = 0;
//            ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "reopening logs");
//            ngx_reopen_files(cycle, -1);
//        }
//    }
}


static void
ngx_start_garbage_collector(ngx_int_t type) {
#if 0
    ngx_int_t      i;
    ngx_channel_t  ch;

    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "start garbage collector");

    ch.command = NGX_CMD_OPEN_CHANNEL;

    ngx_spawn_process(cycle, ngx_garbage_collector_cycle, NULL,
                      "garbage collector", type);

    ch.pid = ngx_processes[ngx_process_slot].pid;
    ch.slot = ngx_process_slot;
    ch.fd = ngx_processes[ngx_process_slot].channel[0];

    for (i = 0; i < ngx_last_process; i++) {

        if (i == ngx_process_slot
            || ngx_processes[i].pid == -1
            || ngx_processes[i].channel[0] == -1)
        {
            continue;
        }

        ngx_log_debug6(NGX_LOG_DEBUG_CORE, cycle->log, 0,
                      "pass channel s:%d pid:%P fd:%d to s:%i pid:%P fd:%d",
                      ch.slot, ch.pid, ch.fd,
                      i, ngx_processes[i].pid,
                      ngx_processes[i].channel[0]);

        /* TODO: NGX_AGAIN */

        ngx_write_channel(ngx_processes[i].channel[0],
                          &ch, sizeof(ngx_channel_t), cycle->log);
    }
#endif
}


static void
ngx_worker_process_init(ngx_uint_t priority)
{
    sigset_t           set;
    ngx_int_t          n;
    ngx_uint_t         i;
    struct rlimit      rlmt;
//    ngx_core_conf_t   *ccf;
//    ngx_listening_t   *ls;

    ngx_process = NGX_PROCESS_WORKER;

//    ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx, ngx_core_module);
//
//    if (geteuid() == 0) {
//        if (priority && ccf->priority != 0) {
//            if (setpriority(PRIO_PROCESS, 0, ccf->priority) == -1) {
//                ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
//                              "setpriority(%d) failed", ccf->priority);
//            }
//        }
//
//        if (ccf->rlimit_nofile != NGX_CONF_UNSET) {
//            rlmt.rlim_cur = (rlim_t) ccf->rlimit_nofile;
//            rlmt.rlim_max = (rlim_t) ccf->rlimit_nofile;
//
//            if (setrlimit(RLIMIT_NOFILE, &rlmt) == -1) {
//                ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
//                              "setrlimit(RLIMIT_NOFILE, %i) failed",
//                              ccf->rlimit_nofile);
//            }
//        }
//
//        if (ccf->rlimit_core != NGX_CONF_UNSET) {
//            rlmt.rlim_cur = (rlim_t) ccf->rlimit_core;
//            rlmt.rlim_max = (rlim_t) ccf->rlimit_core;
//
//            if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
//                ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
//                              "setrlimit(RLIMIT_CORE, %i) failed",
//                              ccf->rlimit_core);
//            }
//        }

#ifdef RLIMIT_SIGPENDING
        if (ccf->rlimit_sigpending != NGX_CONF_UNSET) {
            rlmt.rlim_cur = (rlim_t) ccf->rlimit_sigpending;
            rlmt.rlim_max = (rlim_t) ccf->rlimit_sigpending;

            if (setrlimit(RLIMIT_SIGPENDING, &rlmt) == -1) {
                ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
                              "setrlimit(RLIMIT_SIGPENDING, %i) failed",
                              ccf->rlimit_sigpending);
            }
        }
#endif
//
//        if (setgid(ccf->group) == -1) {
//            ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
//                          "setgid(%d) failed", ccf->group);
//            /* fatal */
//            exit(2);
//        }
//
//        if (initgroups(ccf->username, ccf->group) == -1) {
//            ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
//                          "initgroups(%s, %d) failed",
//                          ccf->username, ccf->group);
//        }
//
//        if (setuid(ccf->user) == -1) {
//            ngx_log_error(NGX_LOG_EMERG, cycle->log, ngx_errno,
//                          "setuid(%d) failed", ccf->user);
//            /* fatal */
//            exit(2);
//        }
//    }

#if (NGX_HAVE_SCHED_SETAFFINITY)

    if (cpu_affinity) {
        ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0,
                      "sched_setaffinity(0x%08Xl)", cpu_affinity);

        if (sched_setaffinity(0, 32, (cpu_set_t *) &cpu_affinity) == -1) {
            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                          "sched_setaffinity(0x%08Xl) failed", cpu_affinity);
        }
    }

#endif

#if (NGX_HAVE_PR_SET_DUMPABLE)

    /* allow coredump after setuid() in Linux 2.4.x */

    if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0) == -1) {
        ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                      "prctl(PR_SET_DUMPABLE) failed");
    }

#endif

//    if (ccf->working_directory.len) {
//        if (chdir((char *) ccf->working_directory.data) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "chdir(\"%s\") failed", ccf->working_directory.data);
//            /* fatal */
//            exit(2);
//        }
//    }

    sigemptyset(&set);

    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) {
//        ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                      "sigprocmask() failed");
    }

//    ngx_init_temp_number();

    /*
     * disable deleting previous events for the listening sockets because
     * in the worker processes there are no events at all at this point
     */
//    ls = cycle->listening.elts;
//    for (i = 0; i < cycle->listening.nelts; i++) {
//        ls[i].previous = NULL;
//    }
//
//    for (i = 0; ngx_modules[i]; i++) {
//        if (ngx_modules[i]->init_process) {
//            if (ngx_modules[i]->init_process(cycle) == NGX_ERROR) {
//                /* fatal */
//                exit(2);
//            }
//        }
//    }

    for (n = 0; n < ngx_last_process; n++) {

        if (ngx_processes[n].pid == -1) {
            continue;
        }

        if (n == ngx_process_slot) {
            continue;
        }

        if (ngx_processes[n].channel[1] == -1) {
            continue;
        }

        if (close(ngx_processes[n].channel[1]) == -1) {
//            ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                          "close() channel failed");
        }
    }

    // 子进程中关闭channel[0] 使用channel[1]
    if (close(ngx_processes[ngx_process_slot].channel[0]) == -1) {
//        ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
//                      "close() channel failed");
    }

#if 0
    ngx_last_process = 0;
#endif

//    if (ngx_add_channel_event(cycle, ngx_channel, NGX_READ_EVENT,
//                              ngx_channel_handler) == NGX_ERROR)
//    {
//        /* fatal */
//        exit(2);
//    }
}



void delay(int number_of_seconds) {
    // Converting time into milli_seconds
    int milli_seconds = CLOCKS_PER_SEC * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}


void print_sigset(const sigset_t *set, const char *name) {
    printf("sigset[%s] is:\n", name);
    int i;
    for (i = 1; i < NSIG; ++i) {
        if (sigismember(set, i))
            printf("1");
        else
            printf("0");
    }
    putchar('\n');
}
