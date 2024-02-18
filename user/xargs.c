#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include <stddef.h>

/* debug log: previous value 200 causes trap due to limited memory */
#define MAXARGLEN 100

int main(int argc, char *argv[]) {
  char *command;
  char buf;
  int i;
  int parsIndex;
  int read_result;
  char pars[MAXARG][MAXARGLEN];
  char *parsList[MAXARG];

  /* validate user input */
  if (argc < 2) {
    fprintf(2, "xargs: too few arguments\n");
    exit(1);
  }
  command = argv[1];

  /* read inputs line by line and execute for each round */ 
  while (1) {
    /* read inputs from stdin and parse into command args */
    /* first, read the argv arguments and store in pars array */
    parsIndex = argc - 1;
    memset(pars, 0, MAXARG * MAXARGLEN);
    for (i = 1; i < argc; i++) {
      strcpy(pars[i - 1], argv[i]);
    }

    /* second, read the rest arguments from stdin */
    int flag = 0; /* indicate whether there is args preceding */
    int idx = 0;  /* indicate the position of each arg while reading */

    while ((read_result = read(0, &buf, 1)) == 1 && buf != '\n') {
      /* case 1: read a normal char */
      if (buf != ' ') {
        pars[parsIndex][idx++] = buf;
        flag = 1;
      /* case 2: finish read an argument */
      } else if (buf == ' ' && flag == 1) {
        pars[parsIndex][idx] = '\0';
        parsIndex++;
        flag = 0;
        idx = 0; 
      } 
    } 
    if (flag == 1) {
      pars[parsIndex][idx] = '\0';
    }
    
    /* third, break if EOF or Error */
    if (read_result <= 0) {
      /* case EOF */
      if (read_result == 0) {
        break; 
      } else if (read_result < 0) {
        fprintf(2, "xargs: read error\n");
        break;
      }
    }

    /* fourth, execute the command using fork and exec */
    /* copy pars into list of strings format for exec */
    for (i = 0; i < MAXARG - 1; i++) {
      parsList[i] = pars[i];      
    }
    parsList[MAXARG] = NULL;

    if (fork() == 0) {
      exec(command, parsList);
      fprintf(2, "xargs: exec failed\n");
      exit(1); 
    } else {
      wait(NULL);
    }
  }
  exit(0);
}
