#include "shared.h"

extern "C"
{

extern int main (...);

__attribute__ ((visibility("default"))) extern void* tabletos_get_main_function ()
{
  return (void*)&main;
}

__attribute__ ((visibility("default"))) extern int tabletos_startup (void* library, int argc, char** argv, char** env)
{
  try
  {    
    typedef void* (*get_main_fn)();

    get_main_fn get_main = (get_main_fn)dlsym (library, "tabletos_get_main_function");
    
    if (!get_main)
      throw xtl::format_operation_exception ("", "tabletos_get_main_function function not found");      

    typedef int (*main_fn)(...);
    
    main_fn main = (main_fn)get_main ();
    
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
