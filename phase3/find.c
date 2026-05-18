// find.c - recursively search a directory for files matching a name/pattern
// Usage: find <path> <pattern>
// Pattern supports '*' wildcard.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

// Simple wildcard matcher: supports '*' and exact matches.
int
wmatch(const char *pattern, const char *name)
{
  if(*pattern == 0)
    return *name == 0;
  if(*pattern == '*'){
    do {
      if(wmatch(pattern + 1, name))
        return 1;
    } while(*name++);
    return 0;
  }
  if(*name && (*pattern == *name))
    return wmatch(pattern + 1, name + 1);
  return 0;
}

// Extract file name from a path (last component).
char*
fmtname(char *path)
{
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

void
find(char *path, char *pattern)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(wmatch(pattern, fmtname(path)))
      printf(1, "%s\n", path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(2, "find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(2, "find: cannot stat %s\n", buf);
        continue;
      }
      if(st.type == T_FILE){
        if(wmatch(pattern, de.name))
          printf(1, "%s\n", buf);
      } else if(st.type == T_DIR){
        find(buf, pattern);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf(2, "Usage: find <path> <pattern>\n");
    exit();
  }
  find(argv[1], argv[2]);
  exit();
}