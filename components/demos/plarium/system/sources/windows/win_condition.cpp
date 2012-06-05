#include "shared.h"

using namespace plarium::system;

namespace
{

typedef HANDLE pthread_mutex_t;

typedef struct
{
  int waiters_count_;
  // Number of waiting threads.

  CRITICAL_SECTION waiters_count_lock_;
  // Serialize access to <waiters_count_>.

  HANDLE sema_;
  // Semaphore used to queue up threads waiting for the condition to
  // become signaled.

  HANDLE waiters_done_;
  // An auto-reset event used by the broadcast/signal thread to wait
  // for all the waiting thread(s) to wake up and be released from the
  // semaphore.

  size_t was_broadcast_;
  // Keeps track of whether we were broadcasting or signaling.  This
  // allows us to optimize the code if we're just signaling.
} pthread_cond_t;

int current_time ()
{
  return int (clock () * 1000.0 / CLOCKS_PER_SEC);
}

void pthread_cond_init (pthread_cond_t *cv, const void *)
{
  cv->waiters_count_ = 0;
  cv->was_broadcast_ = 0;

  cv->sema_ = CreateSemaphore (NULL,       // no security
                                0,          // initially 0
                                0x7fffffff, // max count
                                NULL);      // unnamed

  if (!cv->sema_)
    raise_error ("::CreateSemaphore");

  InitializeCriticalSection (&cv->waiters_count_lock_);

  cv->waiters_done_ = CreateEvent (NULL,  // no security
                                   FALSE, // auto-reset
                                   FALSE, // non-signaled initially
                                   NULL); // unnamed

  if (!cv->waiters_done_)
  {
    DeleteCriticalSection (&cv->waiters_count_lock_);
    CloseHandle (cv->sema_);

    raise_error ("::CreateEvent");
  }
}

void pthread_cond_destroy (pthread_cond_t* cv)
{
  CloseHandle (cv->waiters_done_);
  CloseHandle (cv->sema_);
  DeleteCriticalSection (&cv->waiters_count_lock_);
}

void pthread_cond_wait (pthread_cond_t *cv, pthread_mutex_t *external_mutex)
{
  // Avoid race conditions.

  EnterCriticalSection (&cv->waiters_count_lock_);

  cv->waiters_count_++;

  LeaveCriticalSection (&cv->waiters_count_lock_);

  // This call atomically releases the mutex and waits on the
  // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
  // are called by another thread.

  DWORD result = SignalObjectAndWait (*external_mutex, cv->sema_, INFINITE, FALSE);

  if (result != WAIT_OBJECT_0)
  {
    EnterCriticalSection (&cv->waiters_count_lock_);

    cv->waiters_count_--;

    LeaveCriticalSection (&cv->waiters_count_lock_);

    raise_error ("::SignalObjectAndWait#1");
  }

  // Reacquire lock to avoid race conditions.
  EnterCriticalSection (&cv->waiters_count_lock_);

  // We're no longer waiting...
  cv->waiters_count_--;

  // Check to see if we're the last waiter after <pthread_cond_broadcast>.
  int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

  LeaveCriticalSection (&cv->waiters_count_lock_);

  // If we're the last waiter thread during this particular broadcast
  // then let all the other threads proceed.
  if (last_waiter)
  {
    // This call atomically signals the <waiters_done_> event and waits until
    // it can acquire the <external_mutex>.  This is required to ensure fairness.
    DWORD result = SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);

    if (result != WAIT_OBJECT_0)
      raise_error ("::SignalObjectAndWait#2");
  }
  else
  {
    // Always regain the external mutex since that's the guarantee we
    // give to our callers.
    DWORD result = WaitForSingleObject (*external_mutex, 0);

    if (result != WAIT_OBJECT_0)
      raise_error ("::WaitForSingleObject");
  }
}

void pthread_cond_timedwait (pthread_cond_t *cv, pthread_mutex_t *external_mutex, int end_time)
{
   // TimedWait() is given an absolute time to wait until. To wait for a
   // relative time from now, use TThread::GetTime(). See POSIX threads
   // documentation for why absolute times are better than relative.
   // Returns 0 if successfully signalled, 1 if time expired.

   // Calculate delta T to obtain the real time to wait for
   DWORD wait_time = (DWORD)(end_time - current_time ());
   // Avoid race conditions.
   EnterCriticalSection (&cv->waiters_count_lock_);
   cv->waiters_count_++;
   LeaveCriticalSection (&cv->waiters_count_lock_);

   // This call atomically releases the mutex and waits on the
   // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
   // are called by another thread.
   DWORD result = SignalObjectAndWait (*external_mutex, cv->sema_, wait_time, FALSE);

   if (result != WAIT_OBJECT_0 && result != WAIT_TIMEOUT)
   {
     EnterCriticalSection (&cv->waiters_count_lock_);

     cv->waiters_count_--;

     LeaveCriticalSection (&cv->waiters_count_lock_);

     raise_error ("::SignalObjectAndWait#1");
   }

   // Reacquire lock to avoid race conditions.
   EnterCriticalSection (&cv->waiters_count_lock_);

   // We're no longer waiting...
   cv->waiters_count_--;

   // Check to see if we're the last waiter after <pthread_cond_broadcast>.
   int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;

   LeaveCriticalSection (&cv->waiters_count_lock_);

   // If we're the last waiter thread during this particular broadcast
   // then let all the other threads proceed.
   if (last_waiter)
   {
     // This call atomically signals the <waiters_done_> event and waits until
     // it can acquire the <external_mutex>.  This is required to ensure fairness.
     DWORD result = SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);

     if (result != WAIT_OBJECT_0 )
       raise_error ("::SignalObjectAndWait#2");
   }
   else
   {
     // Always regain the external mutex since that's the guarantee we
     // give to our callers.
     DWORD result = WaitForSingleObject (*external_mutex, 0);

     if (result != WAIT_OBJECT_0)
       raise_error ("::WaitForSingleObject");
   }
}

void pthread_cond_signal (pthread_cond_t *cv)
{
  EnterCriticalSection (&cv->waiters_count_lock_);

  int have_waiters = cv->waiters_count_ > 0;

  LeaveCriticalSection (&cv->waiters_count_lock_);

  // If there aren't any waiters, then this is a no-op.
  if (have_waiters)
  {
    if (!ReleaseSemaphore (cv->sema_, 1, 0))
      raise_error ("::ReleaseSemaphore");
  }
}

void pthread_cond_broadcast (pthread_cond_t *cv)
{
  // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
  // consistent relative to each other.
  EnterCriticalSection (&cv->waiters_count_lock_);

  int have_waiters = 0;

  if (cv->waiters_count_ > 0) {
    // We are broadcasting, even if there is just one waiter...
    // Record that we are broadcasting, which helps optimize
    // <pthread_cond_wait> for the non-broadcast case.
    cv->was_broadcast_ = 1;
    have_waiters       = 1;
  }

  if (have_waiters) {
    // Wake up all the waiters atomically.
    BOOL result = ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);

    LeaveCriticalSection (&cv->waiters_count_lock_);

    if (!result)
      raise_error ("::ReleaseSemaphore");

    // Wait for all the awakened threads to acquire the counting
    // semaphore.

    DWORD wait_result = WaitForSingleObject (cv->waiters_done_, INFINITE);

    // This assignment is okay, even without the <waiters_count_lock_> held
    // because no other waiter threads can wake up to access it.
    cv->was_broadcast_ = 0;

    if (wait_result != WAIT_OBJECT_0)
      raise_error ("::WaitForSingleObject");
  }
  else
  {
    LeaveCriticalSection (&cv->waiters_count_lock_);
  }
}

}

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
    pthread_cond_init (&impl->condition, 0);
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

//Condition wait
void Condition::Wait (Mutex& lock)
{
  pthread_cond_wait (&impl->condition, (pthread_mutex_t*)lock.Handle ());
}

void Condition::Wait (Mutex& lock, size_t wait_in_milliseconds)
{
  pthread_cond_timedwait (&impl->condition, (pthread_mutex_t*)lock.Handle (), current_time () + wait_in_milliseconds);
}

void Condition::Notify (bool broadcast)
{
  if (broadcast)
  {
    pthread_cond_broadcast (&impl->condition);
  }
  else
  {
    pthread_cond_signal (&impl->condition);
  }
}
