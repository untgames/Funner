#include <cstdio>

#include <windows.h>

#undef CreateWindow
#include <engine/engine.h>

using namespace engine;

namespace
{

const char* ENGINE_LIBRARY_NAME = "funner.dll";

}

//точка входа
int main (int argc, const char* argv [])
{
  HMODULE library = LoadLibrary (ENGINE_LIBRARY_NAME);
  
  if (!library)
  {
    printf ("Can't load engine library\n");
    return 1;
  }
  
  FunnerInitProc FunnerInit = (FunnerInitProc)GetProcAddress (library, "FunnerInit");  
  
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

  //FreeLibrary (library); //библиотека не выгружается, из-за необходимости срабатываний atexit, вызванных в funner.dll
}
