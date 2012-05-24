#include "shared.h"

using namespace plarium::system;

struct Mutex::Impl
{
  pthread_mutex_t mutex;
};

//Constructor / destructor
Mutex::Mutex ()
  : impl (new Impl)
{
  try
  {
    pthread_mutexattr_t attr;

    int status = pthread_mutexattr_init (&attr);
    
    if (status)
      pthread_raise_error ("::pthread_mutexattr_init", status);           
    
    status = pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
    
    if (status)
      pthread_raise_error ("::pthread_mutexattr_settype", status);

    status = pthread_mutex_init (&impl->mutex, &attr);

    if (status)
      pthread_raise_error ("::pthread_mutex_init", status);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Mutex::~Mutex ()
{
  int status = pthread_mutex_destroy (&impl->mutex);

  if (status)
    printf ("Error %s while destroying mutex\n", strerror (status));

  delete impl;
}

//Access lock
void Mutex::Lock ()
{
  int status = pthread_mutex_lock (&impl->mutex);

  if (status)
    pthread_raise_error ("::pthread_mutex_lock", status);
}

void Mutex::Unlock ()
{
  int status = pthread_mutex_unlock (&impl->mutex);

  if (status)
    pthread_raise_error ("::pthread_mutex_unlock", status);
}

void* Mutex::Handle ()
{
  return &impl->mutex;
}
