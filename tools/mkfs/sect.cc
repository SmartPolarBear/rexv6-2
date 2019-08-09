#include "sect.h"
#include <cstdio>
#include <algorithm>
#include <unistd.h>

void sector::wsect(uint sec, void *buf)
{
  if (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
  {
    perror("lseek");
    exit(1);
  }
  if (write(fsfd, buf, BSIZE) != BSIZE)
  {
    perror("write");
    exit(1);
  }
}

void sector::rsect(uint sec, void *buf)
{
  if (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
  {
    perror("lseek");
    exit(1);
  }
  if (read(fsfd, buf, BSIZE) != BSIZE)
  {
    perror("read");
    exit(1);
  }
}