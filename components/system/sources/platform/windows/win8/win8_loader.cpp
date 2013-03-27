#include "shared.h"

using namespace Platform;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace concurrency;

namespace
{

typedef void (*StdoutFn)(const char* buffer, size_t size, void* user_data);

struct StdoutContext
{
  StdoutFn      stdout_handler;
  void*         user_data;
  int           stdout_file;
  volatile bool launching;
};

int redirect_stdout(void)
{
  int pipePair [2];

  static const size_t BUFFER_SIZE = 16*1024;

  if (_pipe (pipePair, BUFFER_SIZE, O_TEXT) != 0)
  {
    perror ("ERROR on pipe");
    return -1;
  }

  _dup2 (pipePair [1], _fileno (stdout));  

//  ios::sync_with_stdio ();

  setvbuf (stdout, NULL, _IONBF, 0);

  return pipePair [0];
}

void dump_thread (StdoutContext& context)
{
  char buffer [100];

  context.stdout_handler ("!!!", 3, context.user_data);

  printf ("Hello world!\n"); fflush (stdout);
  
  while (context.launching)
  {      
//      int bytes_read = read (context.stdout_file, buffer, sizeof (buffer));
    int bytes_read = read (context.stdout_file, buffer, 1);

    stl::string s = common::format ("read=%u bytes\n", bytes_read);

  context.stdout_handler (s.c_str (), s.size (), context.user_data);

    if (!bytes_read)
      break;

    context.stdout_handler (buffer, bytes_read, context.user_data);
  }
}

}

extern "C"
{

extern int main (...);

__declspec(dllexport) int win8_startup (const char* cur_dir, StdoutFn stdout_handler, void* user_data)
{
  try
  {    
    int stdout_file = redirect_stdout ();
    
    if (stdout_file < 0)  
      return 1;
      
//    fcntl (stdout_file, F_SETFL, O_NONBLOCK);  

    char* program_name = "launcher", *env = 0;

    StdoutContext stdout_context;

    stdout_context.stdout_handler = stdout_handler;
    stdout_context.user_data      = user_data;
    stdout_context.stdout_file    = stdout_file;
    stdout_context.launching      = true;

    task<void> dump_task (Windows::System::Threading::ThreadPool::RunAsync (ref new WorkItemHandler([&stdout_context](IAsyncAction^ workItem) { dump_thread (stdout_context); })));

    try
    {
      int result = main (0, &program_name, &env);

      stdout_context.launching = false;

      dump_task.wait ();

      return result;
    }
    catch (...)
    {
      stdout_context.launching = false;
      dump_task.wait ();
      throw;
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n    at win8_startup\n", e.what ());
    return -1;    
  }  
  catch (...)
  {
    printf ("Unknown exception\n    at win8_startup\n");
    return -1;
  }
}

}
