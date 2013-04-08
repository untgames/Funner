#include "shared.h"

using namespace Platform;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace concurrency;

using namespace syslib;
using namespace syslib::win8;

#pragma warning (disable : 4273) //incosistent dll linkage

namespace
{

typedef void (*StdoutFn)(const char* buffer, size_t size, void* user_data);

struct StdoutContext
{
  Mutex       mutex;
  stl::string buffer;
  StdoutFn    stdout_handler;
  void*       user_data;

  StdoutContext () : stdout_handler (), user_data () { buffer.reserve (16384); }
};

StdoutContext stdout_context;

}

extern "C"
{

int win8_vprintf (const char *format, va_list list)
{
  try
  {
    if (!format)
      return 0;

    int size = xtl::xvsnprintf (0, 0, format, list);

    if (size == -1)
      return 0;

    Lock lock (stdout_context.mutex);

    stdout_context.buffer.fast_resize (size);

    int result = xtl::xvsnprintf (&stdout_context.buffer [0], size+1, format, list);

    stdout_context.stdout_handler (stdout_context.buffer.c_str (), stdout_context.buffer.size (), stdout_context.user_data);

    return result;
  }
  catch (...)
  {
    return 0;
  }

  return 0;
}

__declspec(dllexport) int win8_startup (const char* cur_dir, StdoutFn stdout_handler, void* user_data)
{
  try
  {    
    stdout_context.stdout_handler = stdout_handler;
    stdout_context.user_data      = user_data;

    common::FileSystem::SetCurrentDir (common::format ("/std/%s", cur_dir).c_str ());

    start_application ();

    return 0;
  }
  catch (std::exception& e)
  {
    printf ("%s\n    at win8_startup\n", e.what ());
  }  
  catch (...)
  {
    printf ("Unknown exception\n    at win8_startup\n");
  }

  return -1;
}

}
