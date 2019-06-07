#include "xv6/syscall.h"
#include "xv6/traps.h"

.globl invoke_sigret_start
.globl invoke_sigret_end

invoke_sigret_start:
  movl $SYS_sigret, %eax
  int $T_SYSCALL
invoke_sigret_end:
