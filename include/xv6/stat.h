#if !defined(__INCLUDE_XV6_STAT_H)
#define __INCLUDE_XV6_STAT_H

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

//TODO: to solve a align problem, this structure is sized 16 in kernel and 20 in user progs
typedef struct stat {
  int type;  // Type of file
  int dev;     // File system's disk device
  int major;
  int minor;
  int ino;    // Inode number
  int nlink; // Number of links to file
  int size;   // Size of file in bytes
}stat_t;
// typedef struct stat {
//   short type;  // Type of file
//   int dev;     // File system's disk device
//   uint ino;    // Inode number
//   short nlink; // Number of links to file
//   uint size;   // Size of file in bytes
// }stat_t;

#endif // __INCLUDE_XV6_STAT_H
