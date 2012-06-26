#include "shared.h"

using namespace plarium::system;

struct Thread::Impl
{
  pthread_t thread;
};

namespace
{

struct thread_function_data
{
  Thread::ThreadFunction start;
  void* thread_arg;
};

void* thread_function (void* data)
{
  thread_function_data* thread_data = (thread_function_data*)data;

  try
  {
    Thread::ThreadFunction start      = thread_data->start;
    void*                  thread_arg = thread_data->thread_arg;

    delete thread_data;

    return start (thread_arg);
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
      throw sgi_stl::invalid_argument ("thread start function");

      //create thread

    thread_function_data *thread_data = new thread_function_data;

    thread_data->start      = start;
    thread_data->thread_arg = thread_arg;

    int status = pthread_create (&impl->thread, 0, thread_function, thread_data);

    if (status)
      pthread_raise_error ("::pthread_create", status);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Thread::~Thread ()
{
  delete impl;
}

/*
    Wait thread function
*/

int Thread::Join ()
{
  void* exit_code = 0;

  int status = pthread_join (impl->thread, &exit_code);

  if (status)
    pthread_raise_error ("::pthread_join", status);

  return (int)exit_code;
}

/*
    Sleep
*/

void Thread::Sleep (size_t milliseconds)
{
  usleep (milliseconds * 1000);
}

/*
   Current thread id
*/

size_t Thread::GetCurrentThreadId ()
{
  return (size_t)pthread_self ();
}
