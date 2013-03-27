#include "shared.h"

namespace
{

typedef void (*StdoutFn)(const char* buffer, size_t size, void* user_data);

}

extern "C"
{

extern int main (...);

__declspec(dllexport) int win8_entry_point (const char* cur_dir, StdoutFn stdout_handler, void* user_data)
{
  try
  {    
    char* program_name = "launcher", *env = 0;

    stdout_handler ("!!!", 3, user_data); //test!

    return main (0, &program_name, &env);
  }
  catch (std::exception& e)
  {
    printf ("%s\n    at win8_entry_point\n", e.what ());
    return -1;    
  }  
  catch (...)
  {
    printf ("Unknown exception\n    at win8_entry_point\n");
    return -1;
  }
}

}
