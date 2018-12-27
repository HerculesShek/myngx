
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// todo will
int main() {
    int pipe_in[2];
    int pipe_out[2];
    printf("pipe_in[0]:%i, pipe_in[1]:%i\n", pipe_in[0], pipe_in[1]);
    printf("pipe_out[0]:%i, pipe_out[1]:%i\n", pipe_out[0], pipe_out[1]);
    pid_t pid;
    pipe(&pipe_in); // 创建父进程中用于读取数据的管道
    pipe(&pipe_out); // 创建父进程中用于写入数据的管道

    printf("pipe_in[0]:%i, pipe_in[1]:%i\n", pipe_in[0], pipe_in[1]);
    printf("pipe_out[0]:%i, pipe_out[1]:%i\n", pipe_out[0], pipe_out[1]);





    return 0;
}