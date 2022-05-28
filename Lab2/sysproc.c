#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

#ifdef GETPPID
int
sys_getppid(void)
{
    int ppid = 1;

    if (myproc()->parent) {
        ppid = myproc()->parent->pid;
    }
    return ppid;
}
#endif // GETPPID

int
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

int
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

/*
#ifdef PROC_TIMES
# error I implemented a function here called suptime (because it
# error makes me think of supper time.
# error it does all the things of sys_uptime() below. I then have
# error sys_uptime() just call suptime().
#endif // PROC_TIMES
*/

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#ifdef CPS
int
sys_cps(void)
{
    proc_cps();
    return 0;
}
#endif // CPS

#ifdef LOTTERY
// error this is where the sys_renice function should live
int
sys_renice(void)
{
  int pid = 0;
  int nice_value = 0;
  int return_value = 0;
  argint(1, &pid);
  argint(0, &nice_value);
  return_value = renice(nice_value, pid);
  return return_value;
}
#endif // LOTTERY
