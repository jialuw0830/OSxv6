#include "kernel/types.h"
#include "user/user.h"

int main() {
    int p1[2]; // parent -> child
    int p2[2]; // child -> parent
    char buf[1];

    pipe(p1);
    pipe(p2);

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) { // child
        // 1. 先从父进程读
        read(p1[0], buf, 1);
        // 2. 打印收到 ping
        printf("%d: received ping\n", getpid());
        // 3. 回复父进程
        write(p2[1], buf, 1);
        exit(0);
    } else { // parent
        // 1. 发送 ping 给子进程
        buf[0] = 'A';
        write(p1[1], buf, 1);
        // 2. 等待子进程回复
        read(p2[0], buf, 1);
        // 3. 打印收到 pong
        printf("%d: received pong\n", getpid());
        wait(0);
        exit(0);
    }
} 