#include "shared.h"

using namespace plarium::system;

struct Thread::Impl
{
  uintptr_t thread;
};

namespace
{

struct thread_function_data
{
  Thread::ThreadFunction start;
  void* thread_arg;
};

unsigned int __stdcall thread_function (void* data)
{
  thread_function_data* thread_data = (thread_function_data*)data;

  try
  {
    Thread::ThreadFunction start      = thread_data->start;
    void*                  thread_arg = thread_data->thread_arg;

    delete thread_data;

    return (unsigned int)start (thread_arg);
  }
  catch (std::exception& e)
  {
    printf ("Thread exception caught '%s'\n", e.what ());
  }
  catch (...)
  {
    printf ("Thread unknown exception caught\n");
  }

  return 0;
}

}

/*
    Constructor / desctructor
*/

Thread::Thread (ThreadFunction start, void* thread_arg)
  : impl (new Impl)
{
  try
  {
      //check arguments

    if (!start)
      throw std::invalid_argument ("thread start function");

      //create thread

    thread_function_data *thread_data = new thread_function_data;

    thread_data->start      = start;
    thread_data->thread_arg = thread_arg;

    impl->thread = _beginthreadex (0, 0, &thread_function, thread_data, 0, 0);

    if (!impl->thread)
      throw std::runtime_error (utility::format ("::_beginthreadex, error %s", strerror (errno)));
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Thread::~Thread ()
{
  CloseHandle ((HANDLE)impl->thread);

  delete impl;
}

/*
    Wait thread function
*/

int Thread::Join ()
{
  if (WaitForSingleObject ((HANDLE)impl->thread, INFINITE) == WAIT_FAILED)
    raise_error ("::WaitForSingleObject");

  DWORD exit_code;

  GetExitCodeThread ((HANDLE)impl->thread, &exit_code);

  return exit_code;
}

/*
    Sleep
*/

void Thread::Sleep (size_t milliseconds)
{
  ::Sleep (milliseconds);
}


/*
   Current thread id
*/

size_t Thread::GetCurrentThreadId ()
{
  return (size_t)::GetCurrentThreadId ();
}
