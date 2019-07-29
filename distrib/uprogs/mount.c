#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"
#include "xv6/fs.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf(2, "Usage: mount [src] [target]\n");
        exit();
    }
    printf(1, "mount:%s->%s\n", argv[1], argv[2]);
    // mount(argv[1], argv[2], XV6FS);
    mount2(0, argv[2], XV6FS);
    exit();
}
