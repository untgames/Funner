#include "shared.h"

void callback (AsyncResult& result, size_t thread_id)
{
  try
  {
    bool is_creator = Thread::GetCurrentThreadId () == thread_id;      
    
    printf ("result is '%s'\n", result.Result<stl::string> ().c_str ());
    printf ("callback executed in thread %s\n", is_creator ? "creator" : "not creator");
    fflush (stdout);    
    
    if (is_creator)
      Application::Exit (0);  
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}

void log_message (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

stl::string worker (size_t thread_id)
{
  printf ("action executed in thread %s\n", Thread::GetCurrentThreadId () == thread_id ? "creator" : "not creator");
  fflush (stdout);  

  return "Hello world";
}

int main ()
{
  printf ("Results of action_queue_thread_wrapper_result:\n");
  
//  LogFilter filter ("system.*", &log_message);
  
  async_invoke<stl::string> (xtl::bind (&worker, Thread::GetCurrentThreadId ()), ActionThread_Current, xtl::bind (&callback, _1, Thread::GetCurrentThreadId ()));

  Application::Run ();

  return 0;
}
