extern "C" int pthread_mutex_unlock(int *mu);
extern "C" int pthread_mutex_lock(int *mu);
extern "C" int pthread_once(int *oncectl, void (*f)(void));
extern "C" int pthread_cancel(int tid);

int
pthread_once(int *oncectl, void (*f)(void))
{
  if (__sync_bool_compare_and_swap(oncectl, 0, 1))
    (*f)();

  return 0;
}

int
pthread_cancel(int tid)
{
  /*
   * This function's job is to make __gthread_active_p
   * in gcc/gthr-posix95.h return 1.
   */
  return 0;
}

int
pthread_mutex_lock(int *mu)
{
  while (!__sync_bool_compare_and_swap(mu, 0, 1))
    ; /* spin */
  return 0;
}

int
pthread_mutex_unlock(int *mu)
{
  *mu = 0;
  return 0;
}