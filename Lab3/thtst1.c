#include "types.h"
#include "stat.h"
#include "user.h"
#include "victor_thread.h"

#include "thtst.h"

#ifdef VTHREADS
void func1(void *arg);

static int global = 10;

#define IVAL 0x0f0f0f0f
#define FVAL 0Xaeaeaeae

void 
func1(void *arg)
{
    int *i = (int *) ((int *) arg);

    //printf(1, "%s %d\n", __FILE__, __LINE__);
    assert(global == 10);
    assert(*i == IVAL);

    //printf(1, "%s %d\n", __FILE__, __LINE__);

    *i = FVAL;
    global = 100;

    //printf(1, "%s %d\n", __FILE__, __LINE__);
    victor_thread_exit(3);
}
#endif // VTHREADS

int
main(int argc, char **argv)
{
#ifdef VTHREADS
    victor_thread_t vt = NULL;
    int rez = -1;
    int i = IVAL;

    if (argc > 1) {
        kdebug(atoi(argv[1]));
    }
    printf(1, "global before: %d\n", global);
    printf(1, "i before     : %x\n", i);
    rez = victor_thread_create(&vt, func1, &i);

    sleep(2);

    printf(1, "rez          : %x\n", rez);

    rez = victor_thread_join(vt);
    printf(1, "global after : %d\n", global);
    printf(1, "i after      : %x\n", i);
    printf(1, "rez          : %d\n", rez);
    assert(global == 100);
    assert(rez == 3);
    assert(i == FVAL);

    sleep(2);
    printf(1, "Tests passed!\n");
#endif // VTHREADS
    exit();
}
