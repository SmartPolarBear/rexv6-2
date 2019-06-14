/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-14 23:40:57
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-14 23:41:37
 * @ Description:
 */


#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"

int main(int argc, char *argv[])
{
    int ppid = getpid();

    if (fork() == 0)
    {
        uint *nullp = (uint *)0;
        printf(1, "null dereference: ");
        printf(1, "%x %x\n", nullp, *nullp);
        // this process should be killed
        printf(1, "TEST FAILED\n");
        kill(ppid);
        exit();
    }
    else
    {
        wait();
    }

    printf(1, "TEST PASSED\n");
    exit();
}