#include "kernel/types.h"
#include <stddef.h>
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int time;

  if (argv[1] == NULL) {
    printf("please enter the sleep time");
    exit(1);
  }

  time = atoi(argv[1]);
  sleep(time);

  exit(0);
}


