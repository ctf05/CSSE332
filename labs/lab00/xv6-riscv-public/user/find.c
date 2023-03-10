#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"

void find(char *path, char *name, char *currentName)
{
  char buf[512];
  //char *p;
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, 0)) < 0) {
    printf("open problem\n");
    exit(1);
  }
  if(fstat(fd, &st) < 0) {
    printf("stat problem\n");
    close(fd);
    return;
  }
    if (st.type == T_DIR) {
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
          continue;
        }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
          continue;
        }
        if (strlen(path) + 1 + strlen(de.name) + 1 > 512) {
          printf("path too long");
          exit(1);
        }
        strcpy(buf, path);
        strcpy(buf + strlen(buf), "/");
        strcpy(buf + strlen(buf), de.name);
        
        // strcpy(buf, path);
        // p = buf + strlen(buf);
        // *p++ = '/';
        // //strcpy(p, buf);
        // //strcpy(p + strlen(p), de.name);
        // printf("p: %s", p);
        // printf("buf: %s", buf);
        find(buf, name, de.name);
      }
        
  }
  else if (st.type == T_FILE) {

    if (strcmp(currentName, name) == 0) {
      printf("%s\n", path);
    }
}
  close(fd);
}
  

int main(int argc, char **argv)
{
  if (argc != 3) {
    printf("Provide 2 arguments");
    exit(1);
  }
  find(argv[1], argv[2], argv[2]);
  exit(0);
}


