#include "shared.h"

using namespace common;
using namespace render::low_level::opengl;

namespace
{

bool thread_lock_state = false;

}

ThreadLock::ThreadLock ()
{
  Lock ();
}

ThreadLock::~ThreadLock ()
{
  Unlock ();
}

void ThreadLock::Lock ()
{
  if (thread_lock_state)
    RaiseInvalidOperation ("render::low_level::opengl::ThreadLock::ThreadLock", "Thread is already locked");

  thread_lock_state = true;
}

void ThreadLock::Unlock ()
{
  thread_lock_state = false;  
}

bool ThreadLock::IsLocked ()
{
  return thread_lock_state;
}
