#include <cstdio>
#include <dlfcn.h>

#include <engine/engine.h>

using namespace engine;

namespace
{

const char* ENGINE_LIBRARY_NAME = "libfunner.so";

}

//точка входа
int main (int argc, const char* argv [])
{
  void* library = dlopen (ENGINE_LIBRARY_NAME, RTLD_NOW | RTLD_GLOBAL);
  
  if (!library)
  {
    printf ("Can't load engine library\n");
    return 1;
  }
    
  FunnerInitProc FunnerInit = (FunnerInitProc)dlsym (library, "FunnerInit");  
  
  if (!FunnerInit)
  {
    printf ("Bad library (entries not found)\n");
    return 1;
  }

  IEngine* funner = FunnerInit ();

  if (!funner)
  {
    printf ("Funner startup failed!");
    return 1;
  }
  
  if (!funner->ParseCommandLine (argc, argv))
  {
    return 1;
  }

  funner->Run ();

  delete funner;

  //dlclose (library); //библиотека не выгружается, из-за необходимости срабатываний atexit, вызванных в libfunner.so
}
