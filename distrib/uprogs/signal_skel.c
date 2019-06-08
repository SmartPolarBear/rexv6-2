
#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"
#include "xv6/signal.h"

void my_handle_signal(int signum)
{
    printf(1, "Received signal %d\n", signum);

    return;
}

int main(void)
{
    int x = 5;
    int y = 0;

    sighandler_t handler = (sighandler_t)my_handle_signal;

    sigset(SIGFPE, handler);

    x = x / y;

    printf(1, "x = %d\n", x);

    exit();
    return 0;
}
