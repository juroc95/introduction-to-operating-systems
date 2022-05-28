#include "types.h"
#include "stat.h"
#include "user.h"

#ifndef PGSIZE
# define PGSIZE 4096
#endif // PGSIZE

int
main(int argc, char **argv)
{
    void *ptr = NULL;

    ptr = malloc(50);
    ptr = malloc(PGSIZE * 2);
    printf(1, "%p ptr\n", ptr);
    printf(1, "%u pg offset ptr\n", ((unsigned long) ptr) % PGSIZE);

    ptr += (PGSIZE - ((unsigned long) ptr) % PGSIZE);
    printf(1, "%p ptr\n", ptr);
    printf(1, "%u pg offset ptr\n", ((unsigned long) ptr) % PGSIZE);

    exit();
}
