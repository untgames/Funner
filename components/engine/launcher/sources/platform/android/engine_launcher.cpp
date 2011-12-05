#include <cstdio>

#include <stl/auto_ptr.h>

#include <engine/engine.h>

using namespace engine;

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

  funner->Run ();

  return 0;
}
