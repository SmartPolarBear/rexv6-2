#ifndef __INCLUDE_XV6_SIGNAL_H
#define __INCLUDE_XV6_SIGNAL_H

#define SIGKILL (0)
#define SIGFPE  (1)

 typedef void (*sighandler_t)(int);

#endif