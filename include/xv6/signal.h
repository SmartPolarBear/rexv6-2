#ifndef __INCLUDE_XV6_SIGNAL_H
#define __INCLUDE_XV6_SIGNAL_H


#define SIGKILL (0)
#define SIGFPE  (1)

 typedef void (*sighandler_t)(int);

#define DEFAULT_SIGNAL_HANDLER (-1)
#define VALIDATE_HANDLER(handler) (((sighandler_t)handler)!=((sighandler_t)(DEFAULT_SIGNAL_HANDLER)))


#endif