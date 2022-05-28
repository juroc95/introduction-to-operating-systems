#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc < 3) {
    printf(1, "Insufficient argument\n");
    exit();
  }
  renice(atoi(argv[1]), getpid());
  exec(argv[2], &(argv[2]));

  exit();
}
