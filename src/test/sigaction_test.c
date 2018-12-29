#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

void print_sigset(const sigset_t *set);

static void sig_quit(int);

static void delay(int number_of_seconds);


/**
 *   使用 sigaction 函数：
    signal 函数的使用方法简单，但并不属于 POSIX 标准，在各类 UNIX 平台上的实现不尽相同，因此其用途受

    到了一定的限制。而 POSIX 标准定义的信号处理接口是 sigaction 函数，其接口头文件及原型如下：
     #include <signal.h>
     int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

     ◆ signum：要操作的信号。
     ◆ act：要设置的对信号的新处理方式。
     ◆ oldact：原来对信号的处理方式。
     ◆ 返回值：0 表示成功，-1 表示有错误发生。
 * @return
 */
int main() {

    // begin 零 信号集相关操作
//    sigset_t set;
//
//    sigemptyset(&set);
//    sigaddset(&set, SIGCHLD); // 20
//    sigaddset(&set, SIGALRM); // 14
//    sigaddset(&set, SIGIO); // 23
//    sigaddset(&set, SIGINT); // 2
//    print_sigset(&set);
//    sigdelset(&set, SIGINT);
//    print_sigset(&set);

    /**
     * 一个进程的信号屏蔽字规定了当前阻塞而不能递送给该进程的信号集。
     * 调用函数sigprocmask可以检测或更改，或同时进行检测和更改进程的信号屏蔽字。
     * int sigprocmask(int how,const sigset_t *restrict set,sigset_t *restrict oset);
     * 返回值：若成功，返回0，若出错，返回-1
     * 就收三个参数，
     *      第一个是如何设置屏蔽字，
     *      第二个是传入的信号集，若set是一个非空指针，则参数how指示如何修改当前信号屏蔽字 如果set是空指针，则不改变该进程的信号屏蔽字，how的值也无意义
     *      第三个是把原来的屏蔽字保存起来（若oset是非空指针，那么进程的当前信号屏蔽字通过oset返回）
     *
     * sigprocmask()函数能够根据参数how来实现对信号集的操作，操作主要有三种：
        SIG_BLOCK 在进程当前阻塞信号集中添加set指向信号集中的信号，相当于：mask=mask|set
        SIG_UNBLOCK 如果进程阻塞信号集中包含set指向信号集中的信号，则解除对该信号的阻塞，相当于：mask=mask|~set
        SIG_SETMASK 更新进程阻塞信号集为set指向的信号集，相当于mask=set
     */
//    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
//    }
//
//    sigemptyset(&set);
//    print_sigset(&set);
    // end 零 信号集相关操作

//    puts("-------------------------------------");
    // begin 一 信号阻塞相关 设置信号屏蔽字 保留信号屏蔽字
    // 传递SIGQUIT信号通过 `Ctrl+\`
    sigset_t newmask, oldmask, pendmask;
    if (signal(SIGQUIT, sig_quit) == SIG_ERR) { // 给SIGQUIT信号添加处理函数
        printf("can't catch SIGQUIT\n");
    }

    // block SIGQUIT and save current signal mask
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    print_sigset(&newmask);
    print_sigset(&oldmask);
    print_sigset(&pendmask);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        printf("SIG_BLOCK error\n");
    }
    print_sigset(&oldmask);
    printf("Now if you send QUIT sig any times in next 5 seconds, program will ignore QUIT sig!\nDon't believe? you can try...\n");
    delay(5);     //SIGQUIT here will remain pending

    /**
     * sigprocmask设置的是屏蔽字 可以理解为静态的
     * 而当前已经阻塞的信号可以通过sigpending获取 同样的信号 如果在屏蔽字中 无论传入多少次 都按照一次计算
     */
    if (sigpending(&pendmask) < 0) {
        printf("sigpending error\n");
    }
    /**
     * 程序第一次sleep的时候 Ctrl+\ 下面会打印 pendmask的值是 0010000000000000000000000000000
     * 如果不按Ctrl+\ 的话 pendmask的值是 0000000000000000000000000000000 就是目前没有阻塞的信号
     */
    print_sigset(&pendmask);

    if (sigismember(&pendmask, SIGQUIT)) {
        printf("\nSIGQUIT pending\n");
    }

    // Restore signal mask which unblocks SIGQUIT
    /**
     * 如果前面按了Ctrl+\ 的话 恢复oldmask之后 QUIT信号会被放行 然后就会收到 sig_quit函数会被调用
     * 这时我们发现，之前如果发送多个SIGQUIT 只会执行一次，说明信号在该系统中没有进行排队
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        printf("SIG_SETMASK error\n");
    }
    printf("SIGQUIT unblocked\n \nIf you send QUIT sig pragram will exit...\n");

    delay(10);
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
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {    //restablished new handler
        printf("can't reset SIGQUIT\n");
    }
}
