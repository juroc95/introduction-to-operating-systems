#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

#ifdef VTHREADS

#include "victor_thread.h"

#ifndef PGSIZE
// copied from mmu.h
# define PGSIZE 4096
#endif // PGSIZE

#ifndef NULL
# define NULL 0x0
#endif // NULL

struct victor_thread_s {
    int vid;
    void *vt_stack;
    void *mem_stack;
};

extern int kthread_create(void (*func)(void *), void *, void *);
extern int kthread_join(int);
extern int kthread_exit(int);

static struct victor_thread_s *vt_new(void);

int
victor_thread_create(victor_thread_t *avt, void (*func)(void*), void *arg_ptr)
{
    struct victor_thread_s *vt = vt_new();
    int result = -1;

    vt->vid = kthread_create(func, arg_ptr, vt->vt_stack);
    //assert(vt->vid == kthread_current());
    //printf(1, "\n%s %d: new thread %d\n", __FILE__, __LINE__, vt->vid);

    if (vt->vid != 0) {
        *avt = (victor_thread_t) vt;
        result = 0;
    }
    return result;
}

int
victor_thread_vid(victor_thread_t avt)
{
    struct victor_thread_s *vt = (struct victor_thread_s *) avt;

    return vt->vid;
}

int
victor_thread_join(victor_thread_t avt)
{
    struct victor_thread_s *vt = (struct victor_thread_s *) avt;
    int retVal = -1;
    
    retVal = kthread_join(vt->vid);
    if (retVal == 0) {
        free(vt->mem_stack);
        vt->vt_stack = vt->mem_stack = NULL;
        free(vt);
    }
    
    return retVal;
}

int
victor_thread_exit(int exit_value)
{
    // This function never returns. As part of the exit, the thread
    // disables itself from running again and calls the 
    // scheduler().
    return kthread_exit(exit_value);
}

static struct victor_thread_s *
vt_new(void)
{
    struct victor_thread_s *vt = malloc(sizeof(struct victor_thread_s));

    if (vt == NULL) {
        return NULL;
    }

    // allocate 2 pages worth of memory and then make sure the
    // beginning address used for the stack is page alligned.
    // we want it page alligned so that we don't generate a
    // page fault by accessing the stack for a thread.
    vt->vt_stack = vt->mem_stack = malloc(PGSIZE * 2);
    if (vt->vt_stack == NULL) {
        free(vt);
        return NULL;
    }
    if (((uint) vt->vt_stack) % PGSIZE != 0) {
        // allign the thread stack to a page boundary
        vt->vt_stack += (PGSIZE - ((uint) vt->vt_stack) % PGSIZE);
    }
    vt->vid = -1;

    return vt;
}

#endif // VTHREADS
