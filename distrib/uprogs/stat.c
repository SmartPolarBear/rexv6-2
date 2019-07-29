#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf(1, "usage:stat [file].\n");
    }

    char *fname = argv[1];
    printf(1, "file:%s\n", fname);

    stat_t st;
    int fd;

    if ((fd = open(fname, 0)) < 0)
    {
        printf(2, "stat: cannot open %s\n", fname);
        exit();
    }

    if ((fstat(fd, &st)) < 0)
    {
        printf(2, "stat: cannot stat %s\n", fname);
        exit();
    }

    printf(1, "type:%d\ndev:%d\nmajor:%d\nminor:%d\nino:%d\nnlink:%d\nsize:%d\n",
           (int)st.type, (int)st.dev, (int)st.major, (int)st.minor, (int)st.ino, (int)st.nlink, (int)st.size);

    close(fd);

    return 0;
}