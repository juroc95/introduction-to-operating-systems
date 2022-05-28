#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
#ifdef KDEBUG
    int kdebug_value = 0;

    printf(1, "%d %s\n", __LINE__, __FILE__);
    if (argc > 1) {
        kdebug_value = atoi(argv[1]);
        if (kdebug_value < 0) {
            printf(1, "debug value must be >= 0\n");
            exit();
        }
    }
    kdebug(kdebug_value);
#endif // KDEBUG
    exit();
}
