#include "types.h"
#include "stat.h"
#include "user.h"
#include "victor_thread.h"

#include "thtst.h"

#ifdef VTHREADS
void func1(void *arg);

extern int kthread_create(void (*func)(void *), void *, void *);
extern void kthread_exit(int);

void 
func1(void *arg)
{
    int i = (int) ((long) arg);


    kthread_exit(i);
}
#endif // VTHREADS

int
main(int argc, char **argv)
{
#ifdef VTHREADS
    char *ptr = malloc(20);
    int rez = -17;

    ptr = malloc(20);

    rez = kthread_create(func1, NULL, ptr);
    printf(1, "%s %d: %d\n", __FILE__, __LINE__, rez);
    assert(rez == -1);
    printf(1, "All tests passed\n");
#endif // VTHREADS
    exit();
}
