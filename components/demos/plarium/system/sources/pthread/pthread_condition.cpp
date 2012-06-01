#include "shared.h"

using namespace plarium::system;

struct Condition::Impl
{
  pthread_cond_t condition;
};

//Constructor / destructor
Condition::Condition ()
  : impl (new Impl)
{
  try
  {
    int status = pthread_cond_init (&impl->condition, 0);

    if (status)
      pthread_raise_error ("::pthread_cond_init", status);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Condition::~Condition ()
{
  int status = pthread_cond_destroy (&impl->condition);

  if (status)
    printf ("Error %s while destroying condition\n", strerror (status));

  delete impl;
}

//Condition wait
void Condition::Wait (Mutex& lock)
{
  int status = pthread_cond_wait (&impl->condition, (pthread_mutex_t*)lock.Handle ());

  if (status)
    pthread_raise_error ("::pthread_cond_wait", status);
}

void Condition::Wait (Mutex& lock, size_t wait_in_milliseconds)
{
  timespec end_time;

#ifdef __APPLE__
  struct timeval current_time;

  gettimeofday (&current_time, 0);

  end_time.tv_sec  = current_time.tv_sec;
  end_time.tv_nsec = 0;
#else
  clock_gettime (CLOCK_REALTIME, &end_time);
#endif

  unsigned long long nsec = (wait_in_milliseconds % 1000) * 1000000 + (unsigned long long)end_time.tv_nsec;

  end_time.tv_nsec  = nsec % 1000000000;
  end_time.tv_sec   = end_time.tv_sec + (time_t)(wait_in_milliseconds / 1000 + nsec / 1000000000);

  int status = pthread_cond_timedwait (&impl->condition, (pthread_mutex_t*)lock.Handle (), &end_time);

  if (status == ETIMEDOUT)
    return;

  if (status)
    pthread_raise_error ("::pthread_cond_timedwait", status);
}

void Condition::Notify (bool broadcast)
{
  if (broadcast)
  {
    int status = pthread_cond_broadcast (&impl->condition);

    if (status)
      pthread_raise_error ("::pthread_cond_broadcast", status);
  }
  else
  {
    int status = pthread_cond_signal (&impl->condition);

    if (status)
      pthread_raise_error ("::pthread_cond_signal", status);
  }
}
