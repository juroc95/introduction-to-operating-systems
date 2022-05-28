#include "types.h"
#include "user.h"

int
main(int argc, char **argv)
{
	srand(3);
	for (int i = 0; i < 10; ++i) {
		printf(1, "random number is: %d\n", rand());
	}
	exit();
}
