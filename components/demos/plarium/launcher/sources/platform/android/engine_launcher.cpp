#include <launcher/application.>

using namespace engine;
using namespace plarium::launcher;

//точка входа
int main (int argc, const char* argv [], const char* env [])
{
  stl::auto_ptr<IEngine> funner (FunnerInit ());

  if (!funner)
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
