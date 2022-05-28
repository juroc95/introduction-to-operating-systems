#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#ifdef HALT
int
sys_halt(void)
{
    outw(0x604, 0x2000);
    return 0;
}
#endif // HALT

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

#ifdef KTHREADS
// error This is a good idea, but not __necessary__ for the tests that we run.
  struct proc *curproc = myproc();

  if (curproc->is_thread) {
      curproc = curproc->parent;
  }
#endif // KTHREADS
  if(argint(0, &n) < 0)
    return -1;
#ifdef KTHREADS
// error addr needs to be from curproc not myproc. It needs to be
// error from thread 0.
	addr = curproc->sz;
#else // KTHREADS
  addr = myproc()->sz;
#endif // KTHREADS
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

#ifdef PROC_TIMES
int
suptime(void)
{
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}
#endif // PROC_TIMES

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
#ifdef PROC_TIMES
    return suptime();
#else // PROC_TIMES
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
#endif // PROC_TIMES
}

#ifdef CPS
int
sys_cps(void)
{
    proc_cps();
    return 0;
}
#endif // CPS

#ifdef KDEBUG
int
sys_kdebug(void)
{
    int n = 0;

    cprintf("%d %s\n", __LINE__, __FILE__);
    if (argint(0, &n) < 0) {
        return -1;
    }
    return proc_kdebug(n);
}
#endif // KDEBUG

#ifdef VA2PA
int
sys_va2pa(void)
{
    int va = 0x0;
    char *pa = 0x0;
    struct proc *p = myproc();

    if (argint(0, &va) < 0) {
        return 0x0;
    }
    pa = uva2ka(p->pgdir, (char *) va);
    pa = pa + (((uint) va) & 0xFFF);
    return ((int) pa);
}
#endif // VA2PA

#ifdef KTHREADS
// error The definitions for the sys_kthread_* functions go in
// error here. They will pop off there paremeters and call the
// error implementions in the proc.c file.

// error the sys_kthread_create has 3 pointers as arguments
int
sys_kthread_create(void)
{
	void (*ptr1) (void*) = NULL;
	void *ptr2 = NULL;
	void *ptr3 = NULL;

	argptr(0, (void *) &ptr1, sizeof(void*));
	argptr(1, (void *) &ptr2, sizeof(void*));
	argptr(2, (void *) &ptr3, sizeof(void*));

	return kthread_create(ptr1, ptr2, ptr3);
}

// error sys_kthread_join has a single int as an argument
int
sys_kthread_join(void)
{
	int num = 0;
	argint(0, &num);
	return kthread_join(num);
}

// error sys_kthread_exit has a single int as an argument
int
sys_kthread_exit(void)
{
	int num = 0;
	argint(0, &num);
	kthread_exit(num);
	return 0;
}

#endif // KTHREADS
