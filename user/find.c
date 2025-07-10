#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    fd = open(path, 0);
    // printf("fd: %d\n", fd);
    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        // 不是目录，直接判断文件名
        if (strcmp(path + strlen(path) - strlen(target), target) == 0) {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }

    // 是目录，遍历
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        // 跳过 . 和 ..
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;
        // 构造新路径
        memset(buf, 0, sizeof(buf));
        strcpy(buf, path);
        p = buf + strlen(buf);
        if (*(p-1) != '/')
            *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        // 获取新路径的 stat
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_DIR) {
            // 递归
            find(buf, target);
        } else {
            // 文件名匹配
            // de.name 可能不是以0结尾，所以要用 memcmp
            if (strcmp(de.name, target) == 0)
                printf("%s\n", buf);
        }
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "usage: find <path> <target>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
