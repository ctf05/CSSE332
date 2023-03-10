#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv)
{
  if (argc == 1) {
    fprintf(2, "Provide an argument");
    exit(1);
  }
  int n = atoi(argv[1]);
  if (n < 0) {
    fprintf(2, "Provide a positive argument");
    exit(1);
  }
  sleep(n);
  exit(0);
}
