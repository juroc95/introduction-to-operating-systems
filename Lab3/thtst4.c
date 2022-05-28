#include "types.h"
#include "stat.h"
#include "user.h"
#include "victor_thread.h"

#include "thtst.h"

#ifdef VTHREADS
#ifndef DEFAULT_NUM_THREADS
# define DEFAULT_NUM_THREADS 10
#endif // DEFAULT_NUM_THREADS

static victor_thread_t vt[MAX_THREADS] = {0};

void func1(void *);
void func2(void *);
void func3(void *);

void
func1(void *arg_ptr)
{
    long arg = ((long) arg_ptr);
    int i = 0;
    int sum = 0;

    printf(1, "Thread %d: %s\n", arg, __FUNCTION__);
    victor_thread_create(&(vt[1]), func2, (void *) (2));
    for (i = 0; i < 320000000; i++) {
        sum ++;
        if (sum % (10000 * 10000) == 0) {
            sum++;
        }
        if (sum > (MAXINT / 2)) {
            sum = 0;
        }
    }
    printf(1, "  delay: %d  %d\n", getpid(), sum);
    
    victor_thread_exit(1);
}

void
func2(void *arg_ptr)
{
    long arg = ((long) arg_ptr);
    
    victor_thread_create(&(vt[2]), func3, (void *) (3));
    printf(1, "\n  Thread %d\n", arg);

    victor_thread_exit(2);
}

void
func3(void *arg_ptr)
{
    long arg = ((long) arg_ptr);

    printf(1, "\n  Thread %d\n", arg);

    victor_thread_exit(3);
}
#endif // VTHREADS

int
main(int argc, char **argv)
{
#ifdef VTHREADS
    int pid = -1;
    int rez = 0;

    pid = fork();
    if (pid == 0) {
        victor_thread_create(&(vt[0]), func1, (void *) 1);
        //long_time(20);
        sleep(5);
        cps();
        
        rez = victor_thread_join(vt[0]);
        printf(1, "%d %d\n", __LINE__, rez);
        assert(rez == 1);
        rez = victor_thread_join(vt[1]);
        printf(1, "%d %d\n", __LINE__, rez);
        assert(rez == 2);
        rez = victor_thread_join(vt[2]);
        printf(1, "%d %d\n", __LINE__, rez);
        assert(rez == 3);
       
        printf(1, "All threads joined\n");
        printf(1, "All tests passed\n");
    }
#endif // VTHREADS
    exit();
}
