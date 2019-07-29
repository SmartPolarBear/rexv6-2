// init: The initial user-level program

#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"
#include "xv6/fcntl.h"
#include "xv6/devnum.h"

char *argv[] = {"sh", 0};

int main(void)
{
    int pid, wpid;
    mkdir("dev");
    chdir("dev");
    mknod("hdainfo", NDEVHDAINFO, MDEVHDAINFO);
    // mknod("hda", NDEVHDA, MDEVHDA);
    mknod("hdap0", NDEVHDA, MDEVHDAP1);
    mknod("hdap1", NDEVHDA, MDEVHDAP2);
    mknod("hdap2", NDEVHDA, MDEVHDAP3);
    mknod("hdap3", NDEVHDA, MDEVHDAP3);

    if (open("console", O_RDWR) < 0)
    {
        mknod("console", NCONSOLE, MCONSOLE);
        open("console", O_RDWR);
    }
    
    mknod("perfctr", NDEVPERFCTR, MDEVPERFCTR);
    mknod("sound", NDEVSOUND, MDEVSOUND);
    mknod("null", NDEVNULL, MDEVNULL);
    mknod("zero", NDEVZERO, MDEVZERO);
    mknod("random", NDEVRANDOM, MDEVRANDOM);
    mknod("urandom", NDEVURANDOM, MDEVURANDOM);
    mknod("full", NDEVFULL, MDEVFULL);
    dup(0); // stdout
    dup(0); // stderr
    chdir("..");
    for (;;)
    {
        printf(1, "init: starting sh\n");
        pid = fork();
        if (pid < 0)
        {
            printf(1, "init: fork failed\n");
            exit();
        }
        if (pid == 0)
        {
            exec("sh", argv);
            printf(1, "init: exec sh failed\n");
            exit();
        }
        while ((wpid = wait()) >= 0 && wpid != pid)
            printf(1, "zombie!\n");
    }
}
