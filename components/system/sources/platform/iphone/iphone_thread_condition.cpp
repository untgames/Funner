#include "shared.h"

using namespace syslib;

namespace
{

void pthread_raise_error (const char* source, int status)
{
  throw xtl::format_operation_exception (source, "Operation failed. Reason: %s (function exit with code %d)", strerror (status), status);
}

/*
   Мьютекс события нити
*/

class ConditionMutex
{
  friend class syslib::Condition;

  public:
    //Конструктор/деструктор
    ConditionMutex ()
    {
      try
      {
        pthread_mutexattr_t attributes;

        pthread_mutexattr_init (&attributes);

        int status = pthread_mutexattr_settype (&attributes, PTHREAD_MUTEX_RECURSIVE);

        if (status)
          pthread_raise_error ("::pthread_mutexattr_init", status);

        status = pthread_mutex_init (&handle, &attributes);

        pthread_mutexattr_destroy (&attributes);

        if (status)
          pthread_raise_error ("::pthread_mutex_init", status);
      }
      catch (xtl::exception& e)
      {
        e.touch ("ConditionMutex::ConditionMutex");
        throw;
      }
    }

    ~ConditionMutex ()
    {
      pthread_mutex_destroy (&handle);
    }

    //Блокировка/освобождение
    void Lock ()
    {
      try
      {
        int status = pthread_mutex_lock (&handle);

        if (status)
          pthread_raise_error ("::pthread_mutex_lock", status);
      }
      catch (xtl::exception& e)
      {
        e.touch ("ConditionMutex::Lock");
        throw;
      }
    }

    void Unlock ()
    {
      try
      {
        int status = pthread_mutex_unlock (&handle);

        if (status)
          pthread_raise_error ("::pthread_mutex_unlock", status);
      }
      catch (xtl::exception& e)
      {
        e.touch ("ConditionMutex::Unlock");
        throw;
      }
    }

  private:
    ConditionMutex (const ConditionMutex&);             //no impl
    ConditionMutex& operator = (const ConditionMutex&); //no impl

  private:
    pthread_mutex_t handle;
};

/*
   Блокировка нити
*/

class Lock
{
  public:
    //Конструктор/деструктор
    Lock (ConditionMutex& in_mutex)
      : mutex (in_mutex)
    {
      mutex.Lock ();
    }

    ~Lock ()
    {
      try
      {
        mutex.Unlock ();
      }
      catch (...)
      {
      }
    }

  private:
    Lock (const Lock&); //no impl
    Lock& operator = (const Lock&); //no impl

  private:
    ConditionMutex& mutex;
};

}

struct Condition::Impl
{
  ConditionMutex mutex;
  pthread_cond_t condition;
};

/*
   Конструктор/деструктор
*/

Condition::Condition ()
  : impl (new Impl)
{
  try
  {
    int status = pthread_cond_init (&impl->condition, 0);

    if (status)
      pthread_raise_error ("::pthread_cond_init", status);
  }

  catch (xtl::exception& e)
  {
    delete impl;
    e.touch ("Condition::Condition");
    throw;
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Condition::~Condition ()
{
  pthread_cond_destroy (&impl->condition);

  delete impl;
}

/*
   Блокировка/освобождение
*/

void Condition::Wait ()
{
  try
  {
    Lock lock (impl->mutex);

    int status = pthread_cond_wait (&impl->condition, &impl->mutex.handle);

    if (status)
      pthread_raise_error ("::pthread_cond_wait", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("Condition::Wait");
    throw;
  }
}

void Condition::Signal ()
{
  try
  {
    Lock lock (impl->mutex);

    int status = pthread_cond_signal (&impl->condition);

    if (status)
      pthread_raise_error ("::pthread_cond_signal", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("Condition::Signal");
    throw;
  }
}

void Condition::Broadcast ()
{
  try
  {
    Lock lock (impl->mutex);

    int status = pthread_cond_broadcast (&impl->condition);

    if (status)
      pthread_raise_error ("::pthread_cond_broadcast", status);
  }
  catch (xtl::exception& e)
  {
    e.touch ("Condition::Broadcast");
    throw;
  }
}
