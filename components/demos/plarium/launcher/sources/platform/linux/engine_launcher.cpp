#include <cstdio>
#include <dlfcn.h>

#include <launcher/application.h>

using namespace engine;
using namespace plarium::launcher;

namespace
{

const char* ENGINE_LIBRARY_NAME = "libfunner.so";

}

//����� �����
int main (int argc, const char* argv [], const char* env [])
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
  
  if (!funner->ParseCommandLine (argc, argv, env))
  {
    return 1;
  }

  {
    Application application;

    application.Run (funner);
  }

  delete funner;

  //dlclose (library); //���������� �� �����������, ��-�� ������������� ������������ atexit, ��������� � libfunner.so
}