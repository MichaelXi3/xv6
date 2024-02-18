#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target);
char* getFileName(char *path);

int main(int argc, char *argv[]) {
  /* check user input */
  if (argc < 2) {
    printf("find error: too few argument\n");
    exit(1);
  } else if (argc == 2) {
    find(".", argv[1]);
    exit(0);
  } else if (argc == 3) {
    find(argv[1], argv[2]);
    exit(0);
  }
  printf("find error: too much arguments\n");
  exit(1);
}

void find(char *path, char *target) {
  /* first, open the path and get all files stats */
  int fd;
  struct dirent de;
  struct stat st;
  char buf[512], *p;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  /* second, handle the FILE and DIR cases */
  switch(st.type) {
    case T_FILE:
      /* check if current file name matches the target */
      if (strcmp(target, getFileName(path)) == 0) {
        printf("%s\n", getFileName(path));
      }
      break;
  
    case T_DIR:
      /* check for the long path cases */
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: pathname too long\n");
        break;
      }

      /* copy the path string to buf */
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';

      /* traverse through all directory entries */
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        /* skip invalid dentries and do not enter cur dir again */
        if((de.inum == 0) || strcmp(de.name, ".") == 0 || strcmp(de.name, "..")
== 0) {
          continue;
      }
        /* copy filename to create new path */
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = '\0';

        /* get stat of the sub-files based on new path */
        if (stat(buf, &st) < 0) {
          printf("find: cannot get stat %s\n", buf);
          continue;
        }
        /* case 1: sub-dir, use recursion to keep finding */
        if (st.type == T_DIR) {
          find(buf, target);
        /* case 2: file, compare the filename to find target */
        }  else if (st.type == T_FILE) {
          if (strcmp(target, de.name) == 0) {
            printf("%s\n", buf);
          }
        }
      }
      break;
  }  
  close(fd);  
}

char* getFileName(char *path) {
  static char buf[DIRSIZ + 1];
  char *p;

  /* find the first argument after the last slash */
  for (p = path + strlen(path); p >= path && *p != '/'; p--);
  p++;

  /* return blank-padded name */
  if (strlen(p) > DIRSIZ) {
    return p;
  }
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}
