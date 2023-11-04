#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
ugetpid(void)
{
  struct usyscall *u = (struct usyscall *)USYSCALL;
  return u->pid;
}
#define USYSCALL (TRAPFRAME - PGSIZE)

