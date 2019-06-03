
#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"
#include "xv6/signal.h"

 void handle_signal(int signum)
{
    printf(1, "Received signal %d\n", signum);

     return;
} 

 int main(void)
{
    int x = 5;
    int y = 0;

     signal(SIGFPE, handle_signal);

     x = x / y;

     printf(1, "x = %d\n", x);

     exit();
} 