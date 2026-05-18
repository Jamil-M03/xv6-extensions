// search.c - search for a keyword inside a file
// Usage: search <keyword> <file>
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
char buf[1024];
// Returns 1 if 'pattern' appears in 'line', else 0.
int
match(char *line, char *pattern)
{
 char *l, *p;
 for(; *line; line++){
 l = line;
 p = pattern;
 while(*p && *l == *p){
 l++;
 p++;
 }
 if(*p == 0)
 return 1;
 }
 return 0;
}
void
search(int fd, char *pattern, char *filename)
{
 int n, i, start;
 static char line[1024];
 int linelen = 0;
 int lineno = 1;
 while((n = read(fd, buf, sizeof(buf))) > 0){
 for(i = 0; i < n; i++){
 if(buf[i] == '\n' || linelen >= (int)sizeof(line) - 1){
 line[linelen] = 0;
 if(match(line, pattern)){
 printf(1, "%s:%d: %s\n", filename, lineno, line);
 }
 lineno++;
 linelen = 0;
 } else {
 line[linelen++] = buf[i];
 }
 }
 }
 // last line without trailing newline
 if(linelen > 0){
 line[linelen] = 0;
 if(match(line, pattern)){
 printf(1, "%s:%d: %s\n", filename, lineno, line);
 }
 }
 // suppress unused warning
 start = 0; (void)start;
}
int
main(int argc, char *argv[])
{
 int fd;
 if(argc < 3){
 printf(2, "Usage: search <keyword> <file>\n");
 exit();
 }
 if((fd = open(argv[2], 0)) < 0){
 printf(2, "search: cannot open %s\n", argv[2]);
 exit();
 }
 search(fd, argv[1], argv[2]);
 close(fd);
 exit();
}