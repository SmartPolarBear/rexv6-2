
#include "mkfs.h"

// Add the given filename and i-number as a directory entry
void dappend(int dirino, char *name, int fileino)
{
    struct xv6dirent de;

    bzero(&de, sizeof(de));
    de.inum = xshort(fileino);
    strncpy(de.name, name, DIRSIZ);
    iappend(dirino, &de, sizeof(de));
}

// Add a file to the directory with given i-num
void fappend(int dirino, char *filename, struct stat *sb)
{
    char buf[BSIZE];
    int cc, fd, inum;

    // Open the file up
    if ((fd = open(filename, 0)) < 0)
    {
        perror(filename);
        exit(1);
    }

    // Allocate an i-node for the file
    inum = ialloc(T_FILE, sb->st_mtime);

    // Add the file's name to the root directory
    dappend(dirino, filename, inum);

    // Read the file's contents in and write to the filesystem
    while ((cc = read(fd, buf, sizeof(buf))) > 0)
        iappend(inum, buf, cc);

    close(fd);
}

// Given a local directory name and a directory i-node number
// on the image, add all the files from the local directory
// to the on-image directory
void add_directory(int dirino, char *localdir)
{
    DIR *D;
    struct dirent *dent;
    struct stat sb;
    int newdirino;

    D = opendir(localdir);
    if (D == NULL)
    {
        perror(localdir);
        exit(1);
    }
    chdir(localdir);

    while ((dent = readdir(D)) != NULL)
    {

        // Skip . and ..
        if (!strcmp(dent->d_name, "."))
            continue;
        if (!strcmp(dent->d_name, ".."))
            continue;

        if (stat(dent->d_name, &sb) == -1)
        {
            perror(dent->d_name);
            exit(1);
        }

        if (S_ISDIR(sb.st_mode))
        {
            newdirino = makdir(dirino, dent->d_name, &sb);
            add_directory(newdirino, dent->d_name);
        }
        if (S_ISREG(sb.st_mode))
        {
            fappend(dirino, dent->d_name, &sb);
        }
    }

    closedir(D);
    chdir("..");
}

// Make a directory entry in the directory with the given i-node number
// and return the new directory's i-number
int makdir(int dirino, char *newdir, struct stat *sb)
{
    int ino;

    // Allocate the inode number for this directory
    // and set up the . and .. entries
    ino = ialloc(T_DIR, sb->st_mtime);
    dappend(ino, ".", ino);
    dappend(ino, "..", dirino);

    // In the parent directory, add the new directory entry
    dappend(dirino, newdir, ino);

    return (ino);
}