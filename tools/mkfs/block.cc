#include "block.h"
#include <cstdio>
#include <cassert>
#include <cstring>

#include "sect.h"

using sector::wsect;

void block::balloc(superblock &sb, int used)
{
  uchar buf[BSIZE];

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < BSIZE * 8);
  memset(buf, 0, sizeof(buf));
  //   bzero(buf, BSIZE);

  for (int i = 0; i < used; i++)
  {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}
