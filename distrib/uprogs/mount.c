#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"
#include "xv6/fs.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf(2, "Usage: mount [dev] [path]\n");
        exit();
    }
    int devid = atoi(argv[1]);
    mkdir(argv[2]);
    mount(devid, argv[2], XV6FS);
    exit();
}
