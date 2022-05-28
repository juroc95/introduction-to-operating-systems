#include "types.h"
#include "stat.h"
#include "user.h"
#include "victor_thread.h"

#include "thtst.h"

#ifdef VTHREADS
void func1(void *arg);

#ifndef DEFAULT_NUM_THREADS
# define DEFAULT_NUM_THREADS 4
#endif // DEFAULT_NUM_THREADS

void 
func1(void *arg_ptr)
{
    long arg = ((long) arg_ptr);
    int i = 0;
    uint max = MAXINT;
    uint sum = 1;

    // this objective for this fuction is to run for some period of time,
    // not toooooo long, but a bit.
    for(i = 1; i < max; i++) {
        sum ++;
        if ((sum % (MAXSHORT * MAXSHORT)) == 0) {
            sum += arg;
            printf(1, "\t%d  thtst2: %d  %d\n\n", arg, getpid(), sum);
        }
        if (sum > (MAXINT / 2)) {
            sum = 0;
        }
    }

    victor_thread_exit(7);
}
#endif // VTHREADS

int
main(int argc, char **argv)
{
#ifdef VTHREADS
    extern int kthread_join(int);

    victor_thread_t vt[MAX_THREADS];
    long i = -1;
    long num_threads = DEFAULT_NUM_THREADS;
    int rez = -17;
    int pid = -1;

    pid = fork();
    if (pid == 0) {
        //printf(1, "%s %d: %p %p\n", __FILE__, __LINE__, main, &argc);
        if (argc > 1) {
            num_threads = atoi(argv[1]);
            if (num_threads < 1 || num_threads > MAX_THREADS) {
                num_threads = DEFAULT_NUM_THREADS;
            }
        }
        printf(1, "Starting %d threads\n", num_threads);
    
        for (i = 0; i < num_threads; i++) {
            victor_thread_create(&(vt[i]), func1, (void *) i);
            printf(1, "%s %d: %d\n", __FILE__, __LINE__, victor_thread_vid(vt[i]));
        }
        cps();
        // the code does not allow you to join with thread 0
        rez = kthread_join(0);
        printf(1, "%s %d: %d\n", __FILE__, __LINE__, rez);
        assert(rez == -1);
        rez = -17;
        rez = kthread_join(50);
        printf(1, "%s %d: %d\n", __FILE__, __LINE__, rez);
        assert(rez == -1);
        
        for (i = 0; i < num_threads; i++) {
            printf(1, "%s %d: joining with %d\n"
                   , __FILE__, __LINE__, victor_thread_vid(vt[i]));
            rez = victor_thread_join(vt[i]);
            assert(rez == 7);
        }

        printf(1, "All threads joined\n");
        printf(1, "All tests passed\n");
    }
#endif // VTHREADS
    exit();
}
