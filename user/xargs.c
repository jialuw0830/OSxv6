// user/xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  char buf[512];
  int i = 0;
  char *params[MAXARG];

  // 先复制命令本体（如 echo bye）到参数数组
  int base = 0;
  for (int j = 1; j < argc; j++) {
    params[base++] = argv[j];
  }

  while (1) {
    char c;
    int n = read(0, &c, 1);
    if (n == 0) break; // 文件结束
    if (c == '\n') {
      buf[i] = 0; // 行结束
      if (i > 0 && buf[0] != 0) {
        // 按空格分割这一行，依次加入参数
        int argi = base;
        char *q = buf;
        while (*q) {
          while (*q == ' ') q++;
          if (*q == 0) break;
          params[argi++] = q;
          while (*q && *q != ' ') q++;
          if (*q) *q++ = 0;
        }
        params[argi] = 0;
        if (fork() == 0) {
          exec(params[0], params);
          fprintf(2, "xargs: exec %s failed\n", params[0]);
          exit(1);
        } else {
          wait(0);
        }
      }
      i = 0; // 重新开始下一行
    } else {
      if (i < sizeof(buf) - 1) buf[i++] = c;
    }
  }

  // 处理最后一行（如果没有以换行结尾）
  if (i > 0 && buf[0] != 0) {
    buf[i] = 0;
    int argi = base;
    char *q = buf;
    while (*q) {
      while (*q == ' ') q++;
      if (*q == 0) break;
      params[argi++] = q;
      while (*q && *q != ' ') q++;
      if (*q) *q++ = 0;
    }
    params[argi] = 0;
    if (fork() == 0) {
      exec(params[0], params);
      fprintf(2, "xargs: exec %s failed\n", params[0]);
      exit(1);
    } else {
      wait(0);
    }
  }

  exit(0);
}
 