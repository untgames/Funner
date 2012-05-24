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
