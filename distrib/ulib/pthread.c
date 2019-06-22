/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-22 00:23:16
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-22 22:44:14
 * @ Description:
 */


#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"
#include "xv6/fs.h"
#include "xv6/fcntl.h"
#include <pthread.h>

int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine) (void*), void* arg)
{
  void* stack = (void*)malloc(4096);
  thread->pid  = clone(start_routine, arg, stack);
  return 0;
}

int pthread_join(pthread_t thread, void** retval)
{
  void* stack;
  join(thread.pid,&stack,retval);
  free(stack);
  return 0;
}

int pthread_exit(void* retval)
{
  texit(retval);
  return 0;
}


int pthread_mutex_destroy(pthread_mutex_t* mutex){
  return mutex_destroy(mutex->id);
}

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr){
  int id = mutex_init();
  mutex->id = id;
  return id;
}

int pthread_mutex_lock(pthread_mutex_t* mutex){
  return mutex_lock(mutex->id);
}

int pthread_mutex_unlock(pthread_mutex_t* mutex){
  return mutex_unlock(mutex->id);
}

