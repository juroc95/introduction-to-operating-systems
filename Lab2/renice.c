#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int nice_value = 0;
  int pid = 0;
  int i = 0;

  if(argc <= 2) {
    printf(1, "Insufficient argument\n");
    exit();
  }

  for(i = 2; i < argc; ++i) {
    nice_value = atoi(argv[1]);
    pid = atoi(argv[i]);
    renice(nice_value, pid);
  }

  exit();
}
