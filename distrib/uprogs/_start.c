#include <time.h>
#include <xv6/user.h>
#include <xv6/signal.h>

int main(int argc, char *argv[]);

void terminal_handler(int pid, int signum)
{
    printf(1, "SIGINT from pid=%d\n\n", pid);
    kill(getpid());

    return;
}

int _start(int argc, char *argv[])
{
    process_start_time = uptime();

    sigset(SIGINT, terminal_handler);

    main(argc, argv);
    exit();
}