/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-16 23:35:05
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-17 23:30:48
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"

#undef NULL
#define NULL ((void*)0)

#define assert(x) if (x) {} else { \
  printf(1, "%s: %d ", __FILE__, __LINE__); \
  printf(1, "assert failed (%s)\n", # x); \
  printf(1, "TEST FAILED\n"); \
  exit(); \
}

void
recurse(int n) 
{
  if(n > 0)
    recurse(n-1);
}

int
main(int argc, char *argv[])
{
  int pid = fork();
  if(pid == 0) {
    // the following command will hit the guardpage, you need to handle the fault and move the guardpage further down
    recurse(1500); // if the fault is not handled, we will not reach the print
    printf(1, "TEST PASSED\n");
    exit();
  } else {
    wait();
  }
  exit();
}