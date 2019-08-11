#if !defined(__INCLUDE_XV6_STAT_H)
#define __INCLUDE_XV6_STAT_H
#if defined(__cplusplus)
extern "C"
{
#endif
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

struct stat {
  short type;  // Type of file
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};
#if defined(__cplusplus)
}
#endif
#endif // __INCLUDE_XV6_STAT_H
