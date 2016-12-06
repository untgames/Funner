#include "shared.h"

const char* LIBRARY_NAME = "data/particle_designer/fireBall_keno_all-scene.plist";

int main ()
{
  printf ("Results of particle_designer_scene_loader_test:\n");
  
  try
  {
    ParticleSystemLibrary library (LIBRARY_NAME);

    //TODO dump info
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
