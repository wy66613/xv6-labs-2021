#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va;  // The starting virtual address of the first user page to check.
  int pgnum;  // The number of pages to check
  uint64 bitmask;  // Use one bit per page
  if(argaddr(0, &va) < 0){
    return -1;
  }
  if(argint(1, &pgnum) < 0){
    return -1;
  }
  if(argaddr(2, &bitmask) < 0){
    return -1;
  }

  if(pgnum > 64){
    return -1;
  }

  struct proc *p = myproc();
  uint64 kbuf = 0; // Kernel buffer
  pte_t *pte;

  for(int i = 0; i < pgnum; i++){
    if(va > MAXVA)
      return -1;

    pte = walk(p->pagetable, va, 0);
    if(pte == 0)
      return -1;

    if(*pte & PTE_A){
      kbuf |= (1 << i);  // Set the page bit in kernel buffer
      *pte &= (~PTE_A);
    }

    va += PGSIZE;  // To the next page
  }

  if(copyout(p->pagetable, bitmask, (char*)&kbuf, sizeof(kbuf)) < 0)
    return -1;

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
