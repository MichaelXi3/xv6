#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  /* pipe from child to parent */
  int p1[2];
  /* pipe from parent to child */
  int p2[2];
  /* char buffer */
  char buf[2];

  pipe(p1);
  pipe(p2);

  if (fork() == 0) {
    /* child read from parent */
    close(p2[1]);
    read(p2[0], buf, 1);
    printf("%d: received ping\n", getpid());
    close(p2[0]);

    /* child write to parent */
    close(p1[0]);
    write(p1[1], buf, 1);
    close(p1[1]);
  } else {
    /* parent write to child */
    close(p2[0]);
    write(p2[1], buf, 1);
    close(p2[1]);

    wait(0);

    /* parent read from child */ 
    close(p1[1]);
    read(p1[0], buf, 1);
    printf("%d: received pong\n", getpid());
    close(p1[0]); 
  }

  exit(0);
}
