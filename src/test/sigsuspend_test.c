#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void print_sigset(const sigset_t *set);

static void sig_quit(int);
static void sig_user1(int);

static void delay(int number_of_seconds);

int main() {

    sigset_t newmask, oldmask, suspend;
    if (signal(SIGQUIT, sig_quit) == SIG_ERR) { // 给SIGQUIT信号添加处理函数
        printf("can't catch SIGQUIT\n");
    }

    if (signal(SIGUSR1, sig_user1) == SIG_ERR) { // 给SIGQUIT信号添加处理函数
        printf("can't catch SIGUSR1\n");
    }

    // block SIGQUIT and save current signal mask
    sigemptyset(&newmask);
    sigemptyset(&oldmask);
    sigemptyset(&suspend);
    // 拦截QUIT信号 挂起程序
    sigaddset(&suspend, SIGQUIT);
    // sigaddset(&newmask, SIGQUIT);
    print_sigset(&newmask);
    print_sigset(&oldmask);
    print_sigset(&suspend);


    //    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
    //        printf("SIG_BLOCK error\n");
    //    }
    //    print_sigset(&oldmask);
    //    printf("Now if you send QUIT sig any times in next 5 seconds, program will ignore QUIT sig!\nDon't believe? you can try...\n");
    //    delay(5);     //SIGQUIT here will remain pending

    /**
     * This function replaces the process’s signal mask with set and then suspends
     * the process until a signal is delivered whose action is either to terminate
     * the process or invoke a signal handling function.
     * In other words, the program is effectively suspended until one of the signals
     * that is not a member of set arrives.
     *
     * If the process is woken up by delivery of a signal that invokes a handler function,
     * and the handler function returns, then sigsuspend also returns.
     */

    int res = sigsuspend(&suspend);
    printf("suspend res is %i\n", res);
//    if (sigsuspend(&suspend) < 0) {
//        printf("suspend error\n");
//    }
    printf("after sigsuspend...\n");

    delay(10);

    while (1){
        printf("waiting for a sig...\n");
        sigsuspend(&oldmask);
    }

    /**
     * 程序第一次sleep的时候 Ctrl+\ 下面会打印 pendmask的值是 0010000000000000000000000000000
     * 如果不按Ctrl+\ 的话 pendmask的值是 0000000000000000000000000000000 就是目前没有阻塞的信号
     */
    //    print_sigset(&pendmask);
    //
    //    if (sigismember(&pendmask, SIGQUIT)) {
    //        printf("\nSIGQUIT pending\n");
    //    }

    // Restore signal mask which unblocks SIGQUIT
    /**
     * 如果前面按了Ctrl+\ 的话 恢复oldmask之后 QUIT信号会被放行 然后就会收到 sig_quit函数会被调用
     * 这时我们发现，之前如果发送多个SIGQUIT 只会执行一次，说明信号在该系统中没有进行排队
     */
    //    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
    //        printf("SIG_SETMASK error\n");
    //    }
    //    printf("SIGQUIT unblocked\n \nIf you send QUIT sig pragram will exit...\n");
    //
    //    delay(10);
    // end 一

    return 0;
}

void delay(int number_of_seconds) {
    // Converting time into milli_seconds
    int milli_seconds = CLOCKS_PER_SEC * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}

void print_sigset(const sigset_t *set) {
    int i;
    for (i = 1; i < NSIG; ++i) {
        if (sigismember(set, i))
            printf("1");
        else
            printf("0");
    }
    putchar('\n');
}

static void sig_quit(int signo) {
    printf("caught SIGQUIT, signo is %i\n", signo);
//    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {    //restablished new handler
//        printf("can't reset SIGQUIT\n");
//    }
}

static void sig_user1(int signo) {
    printf("caught SIGUSER1, signo is %i\n", signo);
}
