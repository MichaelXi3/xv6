#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void getPrimes();

int main(int argc, char *argv[]) {
  int p[2];
  int i;

  pipe(p);

  if (fork() == 0) {
    getPrimes(p);
    exit(0);
  }

  /* parent sends number 2 to 35 to child */
  for (i = 2; i <= 35; i++) {
    close(p[0]);
    if (write(p[1], (void *)&i, sizeof(i)) != 4) {
      printf("parent write into pipe error\n");
      exit(1);
    }
  }  
  close(p[1]);

  /* parent waits entire pipeline to finish */
  while (wait(0) > 0);
  exit(0);
}

void getPrimes(int *p) {
  int prime;
  int num;
  int p1[2];

  /* the first number is prime */
  close(p[1]);
  read(p[0], (void *) &prime, sizeof(prime));
  printf("prime %d\n", prime);

  /* examine the subsequent numbers, filter the non-prime numbers */
  pipe(p1);
  if (read(p[0], (void *)&num, sizeof(num)) != 0) {
    /* child will continue the next round of checking prime */
    if (fork() == 0) {
      getPrimes(p1);
      exit(0);
    }

    /* at the parent side, filter out not prime numbers */
    close(p1[0]);
    do {
      if (num % prime != 0) {
        write(p1[1], (void *)&num, sizeof(num));
      }
    } while (read(p[0], (void *)&num, sizeof(num)) != 0);
    close(p1[1]);
    close(p[0]);
    wait(0);
  } 
}
