#ifndef __VICTOR_THREAD_H
# define __VICTOR_THREAD_H

# ifdef VTHREADS
typedef int victor_thread_t;

int victor_thread_create(victor_thread_t *, void (*func)(void*), void *);
int victor_thread_join(victor_thread_t);
int victor_thread_exit(int);
int victor_thread_vid(victor_thread_t);

# endif // VTHREADS

#endif // __VICTOR_THREAD_H
