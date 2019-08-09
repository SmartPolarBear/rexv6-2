#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "byte_order.h"

using std::cin;
using std::cout;
using std::endl;
using std::min;

using byte_order_intel::xint;
using byte_order_intel::xshort;

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

constexpr int nbitmap = FSSIZE / (BSIZE * 8) + 1;
constexpr int ninodeblocks = NINODES / IPB + 1;
constexpr int nlog = LOGSIZE;

int nmeta = 0;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nblocks = 0; // Number of data blocks

int fsfd = -1;
struct superblock sb = {};
char zeroes[BSIZE] = {0};
uint freeinode = 1;
uint freeblock = 0;

void balloc(int);
void wsect(uint, void *);
void winode(uint, struct dinode *);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

void initialize_superblock();
ushort initialize_rootino();
int add_file(const char *file, int rootino);

int main(int argc, char *argv[])
{
  static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

  if (argc < 2)
  {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    return 1;
  }

  assert((BSIZE % sizeof(struct dinode)) == 0);
  assert((BSIZE % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);

  if (fsfd < 0)
  {
    perror(argv[1]);
    return 1;
  }

  initialize_superblock();

  auto rootino = initialize_rootino();

  for (int i = 2; i < argc; i++)
  {
    auto ret = add_file(argv[i], rootino);
    if (ret != 0)
    {
      fprintf(stderr, "Errors occur in writting file %s.", argv[i]);
      return 1;
    }
  }

  struct dinode din = {};
  // fix size of root inode dir
  rinode(rootino, &din);
  int off = xint(din.size);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  return 0;
}

void initialize_superblock()
{
  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.size = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
         nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  for (int i = 0; i < FSSIZE; i++)
  {
    wsect(i, zeroes);
  }

  char buf[BSIZE];
  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);
}

ushort initialize_rootino()
{
  using std::begin;
  using std::end;
  using std::fill;

  auto rootino = (ushort)ialloc(T_DIR);
  assert(rootino == ROOTINO);

  dirent root = {xshort(rootino)}, root_prev = {xshort(rootino)};

  //dir entry "."
  fill(begin(root.name), end(root.name), '\0');
  strcpy(root.name, ".");
  iappend(rootino, &root, sizeof(root));

  fill(begin(root_prev.name), end(root_prev.name), '\0');
  strcpy(root_prev.name, "..");
  iappend(rootino, &root_prev, sizeof(root_prev));

  return rootino;
}

int add_file(const char *file, int rootino)
{
  using std::begin;
  using std::end;
  using std::fill;

  auto fd = open(file, 0);
  if (fd < 0)
  {
    perror(file);
    return 1;
  }

  //[Deprecated] the name now never statrs with '_'
  //
  // Skip leading _ in name when writing to file system.
  // The binaries are named _rm, _cat, etc. to keep the
  // build operating system from trying to execute them
  // in place of system binaries like rm and cat.
  // if (argv[i][0] == '_')
  //   ++argv[i];

  //skip the path for the file
  auto dispname = strrchr(file, '/');
  dispname = !dispname ? const_cast<char *>(file) : dispname + 1; //skip the '/' char
  assert(index(dispname, '/') == 0);

  auto inum = (ushort)ialloc(T_FILE);

  dirent de = {xshort(inum)};
  fill(begin(de.name), end(de.name), '\0');
  strncpy(de.name, dispname, DIRSIZ);
  iappend(rootino, &de, sizeof(de));

  int cc = 0;
  char buf[BSIZE] = {0};
  while ((cc = read(fd, buf, sizeof(buf))) > 0)
  {
    iappend(inum, buf, cc);
  }

  close(fd);
  return 0;
}

void wsect(uint sec, void *buf)
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

void winode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

void rsect(uint sec, void *buf)
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

uint ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}

void balloc(int used)
{
  uchar buf[BSIZE];
  int i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < BSIZE * 8);
  bzero(buf, BSIZE);
  for (i = 0; i < used; i++)
  {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

void iappend(uint inum, void *xp, int n)
{
  char *p = (char *)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  while (n > 0)
  {
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    if (fbn < NDIRECT)
    {
      if (xint(din.addrs[fbn]) == 0)
      {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    }
    else
    {
      if (xint(din.addrs[NDIRECT]) == 0)
      {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (char *)indirect);
      if (indirect[fbn - NDIRECT] == 0)
      {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (char *)indirect);
      }
      x = xint(indirect[fbn - NDIRECT]);
    }
    n1 = min((uint)n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}
