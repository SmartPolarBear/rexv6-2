#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"

char buf[512];

void cat(int fd)
{
    int n;
    while ((n = read(fd, buf, sizeof(buf))) > 0)
    {
        printf(1, "cat:read n=%d\n", (int)n);
        if (write(1, buf, n) != n)
        {
            printf(1, "cat: write error\n");
            exit();
        }
    }
    printf(1, "cat:read n=%d\n", (int)n);

    if (n < 0)
    {
        printf(1, "cat: read error\n");
        exit();
    }
}

int main(int argc, char *argv[])
{
    int fd, i;

    if (argc <= 1)
    {
        cat(0);
        exit();
    }

    for (i = 1; i < argc; i++)
    {
        if ((fd = open(argv[i], 0)) < 0)
        {
            printf(1, "cat: cannot open %s\n", argv[i]);
            exit();
        }
        printf(1, "cat:fd=%d\n", (int)fd);
        cat(fd);
        close(fd);
    }
    exit();
}
