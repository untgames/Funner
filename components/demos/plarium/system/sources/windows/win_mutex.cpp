#include "shared.h"

using namespace plarium::system;

struct Mutex::Impl
{
  HANDLE mutex;
};

//Constructor / destructor
Mutex::Mutex ()
  : impl (new Impl)
{
  try
  {
    impl->mutex = CreateMutexA (0, FALSE, 0);

    if (!impl->mutex)
      raise_error ("::CreateMutex");
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Mutex::~Mutex ()
{
  CloseHandle (impl->mutex);

  delete impl;
}

//Access lock
void Mutex::Lock ()
{
  DWORD status = WaitForSingleObject (impl->mutex, INFINITE);

  if (status == WAIT_FAILED)
    raise_error ("::WaitForSingleObject");
}

void Mutex::Unlock ()
{
  if (!ReleaseMutex (impl->mutex))
    raise_error ("::ReleaseMutex");
}

void* Mutex::Handle ()
{
  return &impl->mutex;
}
