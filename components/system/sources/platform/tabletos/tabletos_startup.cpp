#include "shared.h"

extern "C"
{

__attribute__ ((visibility("default"))) extern int main (...);

__attribute__ ((visibility("default"))) extern int tabletos_startup (void* library, int argc, char** argv, char** env)
{
  try
  {    
    typedef int (*main_fn)(...);
    
    main_fn main = (main_fn)dlsym (library, "main");
    
    if (!main)
      throw xtl::format_operation_exception ("", "main function not found");
    
    return main (argc, argv, env);
  }
  catch (std::exception& e)
  {
    printf ("%s\n    at tabletos_startup\n", e.what ());
    return -1;    
  }  
  catch (...)
  {
    printf ("Unknown exception\n    at tabletos_startup\n");
    return -1;
  }
}

}
