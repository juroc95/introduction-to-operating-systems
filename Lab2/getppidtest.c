#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
#ifdef GETPPID
    int pid = -1;
    int ppid = getpid();

    pid = fork();
    if (pid == 0) {
        int cppid = getppid();
        assert(ppid == cppid);
        printf(1, "test success: %d %d\n", ppid, cppid);
        exit();
    }
    wait();
#else // GETPPID
    printf(1, "getppid test not run\n");
#endif // GETPPID
    exit();
}
