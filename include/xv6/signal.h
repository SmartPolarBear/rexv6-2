#ifndef __INCLUDE_XV6_SIGNAL_H
#define __INCLUDE_XV6_SIGNAL_H

#define SIGKILL (0)
#define SIGFPE (1)
#define SIGINT (2)

#define SIGUSR1 (30)
#define SIGUSR2 (31)

typedef void (*sighandler_t)(int signum);

#define SIGNAL_MIN (0)
#define SIGNAL_MAX (32)

#define VALIDATE_HANDLER(handler) ((handler) != (sighandler_t)(-1))

#endif