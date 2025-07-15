#include "kernel/types.h"
#include "user/user.h"

int
pgaccess(const void *va, int npages, uint32 *abits)
{
  return syscall(SYS_pgaccess, va, npages, abits);
} 