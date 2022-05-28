#include "types.h"
#include "user.h"

int main(void)
{
#ifdef HALT
    printf(1, "shutting down xv6\n");
    halt();
#endif // HALT
    exit();
}
