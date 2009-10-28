#include <cstdio>

#include <windows.h>

#include <engine/launcher.h>

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

  IFunnerApi* funner = FunnerInit ();

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
