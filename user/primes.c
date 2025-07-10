#include "kernel/types.h"
#include "user/user.h"

void sieve(int p_read) {
    int prime;
    int n;
    int p[2];

    // 读到第一个数，作为本进程的 prime
    if (read(p_read, &prime, sizeof(prime)) == 0) {
        close(p_read);
        exit(0);
    }
    printf("prime %d\n", prime);

    // 创建下一个 pipe
    pipe(p);
    int pid = fork();
    if (pid == 0) {
        // 子进程：递归处理下一个筛选
        close(p[1]);
        sieve(p[0]);
        exit(0);
    } else {
        // 父进程：筛选剩余的数
        close(p[0]);
        while (read(p_read, &n, sizeof(n)) > 0) {
            if (n % prime != 0) {
                write(p[1], &n, sizeof(n));
            }
        }
        close(p_read);
        close(p[1]);
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0) {
        // 子进程：开始 sieve
        close(p[1]);
        sieve(p[0]);
        exit(0);
    } else {
        // 父进程：写入 2~35
        close(p[0]);
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
} 