// split.c - split a file into smaller chunks of a given size (in bytes)
// Usage: split <file> <chunk_size_bytes>
// Produces: <file>.part0, <file>.part1, ...

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Build "<base>.partN" into outname.
void
make_name(char *outname, char *base, int part)
{
  int i = 0, j;
  while(base[i] && i < 50){
    outname[i] = base[i];
    i++;
  }
  j = i;
  outname[j++] = '.'; outname[j++] = 'p'; outname[j++] = 'a';
  outname[j++] = 'r'; outname[j++] = 't';

  // integer to string
  char num[12];
  int k = 0, p = part;
  if(p == 0){
    num[k++] = '0';
  } else {
    char tmp[12]; int t = 0;
    while(p > 0){ tmp[t++] = '0' + (p % 10); p /= 10; }
    while(t > 0) num[k++] = tmp[--t];
  }
  int x;
  for(x = 0; x < k; x++) outname[j++] = num[x];
  outname[j] = 0;
}

int
main(int argc, char *argv[])
{
  int fd_in, fd_out;
  int chunk_size;
  int part = 0;
  int written = 0;
  int n;
  char buf[512];
  char outname[64];

  if(argc < 3){
    printf(2, "Usage: split <file> <chunk_size_bytes>\n");
    exit();
  }

  chunk_size = atoi(argv[2]);
  if(chunk_size <= 0){
    printf(2, "split: invalid chunk size\n");
    exit();
  }

  if((fd_in = open(argv[1], 0)) < 0){
    printf(2, "split: cannot open %s\n", argv[1]);
    exit();
  }

  make_name(outname, argv[1], part);
  if((fd_out = open(outname, O_CREATE | O_WRONLY)) < 0){
    printf(2, "split: cannot create %s\n", outname);
    close(fd_in);
    exit();
  }

  while((n = read(fd_in, buf, sizeof(buf))) > 0){
    int offset = 0;
    while(offset < n){
      int remaining = chunk_size - written;
      int to_write = n - offset;
      if(to_write > remaining) to_write = remaining;

      if(write(fd_out, buf + offset, to_write) != to_write){
        printf(2, "split: write error\n");
        close(fd_in); close(fd_out);
        exit();
      }
      written += to_write;
      offset += to_write;

      if(written >= chunk_size){
        close(fd_out);
        part++;
        written = 0;

        if(offset < n){
          make_name(outname, argv[1], part);
          if((fd_out = open(outname, O_CREATE | O_WRONLY)) < 0){
            printf(2, "split: cannot create %s\n", outname);
            close(fd_in);
            exit();
          }
        }
      }
    }
  }

  if(written > 0){
    close(fd_out);
    part++;
  }
  close(fd_in);
  printf(1, "split: created %d part(s)\n", part);
  exit();
}