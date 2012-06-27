#include <stdio.h>
#include <launcher/application.h>
#include <sgi_stl/memory>

using namespace engine;
using namespace plarium::launcher;

//точка входа
int main (int argc, const char* argv [], const char* env [])
{
  sgi_stl::auto_ptr<IEngine> funner (FunnerInit ());

  if (!funner.get ())
  {
    printf ("Funner startup failed!");
    return 1;
  }

  if (!funner->ParseCommandLine (argc, argv, env))
    return 1;

  Application application;

  application.Run (funner.get ());

  return 0;
}
