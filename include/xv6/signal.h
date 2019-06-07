#ifndef __INCLUDE_XV6_SIGNAL_H
#define __INCLUDE_XV6_SIGNAL_H

#define SIGKILL (0)
#define SIGFPE (1)

typedef void (*sighandler_t)(int spid, int signum);

#define SIGNAL_MIN (0)
#define SIGNAL_MAX (32)

#define VALIDATE_HANDLER(handler) ((handler) != (sighandler_t)(-1))

#endif