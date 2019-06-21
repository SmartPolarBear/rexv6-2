/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-01 23:56:40
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-22 00:16:32
 * @ Description:
 */

#ifndef __INCLUDE_XV6_SYSCALL_H
#define __INCLUDE_XV6_SYSCALL_H

//error success is sucess.
#define ERROR_SUCCESS (0)

//handle when trying to retreving more arguments than given.
#define ERROR_OUT_OF_STACK (-1)

//corresponding to null dereference handling
//TODO: HANDLE IT WITH SIGNAL
#define ERROR_IN_FIRST_PAGE (-2)

//value invalid, like non-NUL-terminated strs or bad pointers.
#define ERROR_INVALID_VAL (-3)

// System call numbers
#define SYS_fork 1
#define SYS_exit 2
#define SYS_wait 3
#define SYS_pipe 4
#define SYS_read 5
#define SYS_kill 6
#define SYS_exec 7
#define SYS_fstat 8
#define SYS_chdir 9
#define SYS_dup 10
#define SYS_getpid 11
#define SYS_sbrk 12
#define SYS_sleep 13
#define SYS_uptime 14
#define SYS_open 15
#define SYS_write 16
#define SYS_mknod 17
#define SYS_unlink 18
#define SYS_link 19
#define SYS_mkdir 20
#define SYS_close 21

#define SYS_lock_create 31
#define SYS_lock_acquire 32
#define SYS_lock_release 33
#define SYS_lock_holding 34
#define SYS_lock_free 35

#define SYS_semaphore_create 41
#define SYS_semaphore_acquire 42
#define SYS_semaphore_release 43
#define SYS_semaphore_getcounter 44
#define SYS_semaphore_free 45

#define SYS_rwlock_create 51
#define SYS_rwlock_acquire_read 52
#define SYS_rwlock_acquire_write 53
#define SYS_rwlock_release_read 54
#define SYS_rwlock_release_write 55
#define SYS_rwlock_holding_read 56
#define SYS_rwlock_holding_write 57
#define SYS_rwlock_free 58

#define SYS_getcrtc 59
#define SYS_setcrtc 60
#define SYS_getcurpos 61
#define SYS_setcurpos 62
#define SYS_geteditstatus 63
#define SYS_seteditstatus 64
#define SYS_mount 65
#define SYS_unmount 66
#define SYS_gettime 67

#define SYS_getcrtc 68
#define SYS_setcrtc 69
#define SYS_getcurpos 70
#define SYS_setcurpos 71
#define SYS_geteditstatus 72
#define SYS_seteditstatus 73
#define SYS_mount 74
#define SYS_unmount 75
#define SYS_gettime 76
#define SYS_playsound 77
#define SYS_nosound 78

#define SYS_settickets (79)

#define SYS_sigset (80)
#define SYS_sigsend (81)
#define SYS_sigret (82)
#define SYS_sigpause (83)

#define SYS_clone (22)
#define SYS_join (23)
#define SYS_texit (24)
#define SYS_mutex_init (84)
#define SYS_mutex_destroy (85)
#define SYS_mutex_lock (86)
#define SYS_mutex_unlock (87)
#endif