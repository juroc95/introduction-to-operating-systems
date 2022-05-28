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
    int j = 0;
    uint max = MAXINT;
    uint sum = 1;

    printf(1, "\tThread %d: started\n\n", arg);
    // this objective for this fuction is to run for some period of time,
    // not toooooo long, but a bit.
    for (j = 0; j < max; j++) {
        for(i = 1; i < 4 ; i++) {
            sum ++;
            if (sum % (MAXSHORT * MAXSHORT) == 0) {
                printf(1, "\t\tThread %d: %d\n\n", arg, sum);
            }
            if (sum > (MAXINT / 2)) {
                sum = 0;
            }
        }
    }

    printf(1, "\tThread %d: done\n\n", arg);
    victor_thread_exit(arg);
}
#endif // VTHREADS

int
main(int argc, char **argv)
{
#ifdef VTHREADS
    victor_thread_t vt[MAX_THREADS];
    long i = -1;
    long num_threads = DEFAULT_NUM_THREADS;
    int pid = 0;
    int vt_ret = 0;

    //printf(1, "%s %d: %p %p\n", __FILE__, __LINE__, main, &argc);
    if (argc > 1) {
        num_threads = atoi(argv[1]);
        if (num_threads < 1 || num_threads > MAX_THREADS) {
            num_threads = DEFAULT_NUM_THREADS;
        }
    }
    printf(1, "Starting %d threads\n", num_threads);

    pid = fork();
    if (pid == 0) {
        for (i = 0; i < num_threads; i++) {
            victor_thread_create(&(vt[i]), func1, (void *) i);
            printf(1, "%s %d: started thread %d\n"
                   , __FILE__, __LINE__, victor_thread_vid(vt[i]));
        }
        cps();
        for (i = 0; i < num_threads; i++) {
            printf(1, "%s %d: joining with %d\n"
                   , __FILE__, __LINE__, victor_thread_vid(vt[i]));
            vt_ret = victor_thread_join(vt[i]);
            printf(1, "Thread %d exit value %d\n", i, vt_ret);
            if (vt_ret != i) {
                printf(1, "    Thread %d exit value %d not what expected. Killed?\n", i, vt_ret);
            }
        }

        printf(1, "All threads joined\n");
        printf(1, "All tests passed!\n");
    }
#endif // VTHREADS
    exit();
}
