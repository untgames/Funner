#include "shared.h"

const char* LIBRARY_NAMES [] = { "data/particle_designer/fire big.plist", "data/particle_designer/fireBall_keno_all-scene.plist" };

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of particle_designer_emitter_loader_test:\n");

  LogFilter log_filter ("*", &print_log);

  for (size_t current_library = 0, count = sizeof (LIBRARY_NAMES) / sizeof (*LIBRARY_NAMES); current_library < count; current_library++)
  {
    printf ("\nTesting library '%s':\n", LIBRARY_NAMES [current_library]);

    try
    {
      ParticleSystemLibrary library (LIBRARY_NAMES [current_library]);

      //TODO dump info

      RandomGenerator random;

      for (ParticleSystemLibrary::Iterator iter = library.CreateIterator (); iter; ++iter)
      {
        ParticleSystem system (random);

        (*iter)->Configure (system);

        printf ("simulation:\n");
        TimeValue tv;

        for (size_t i = 0; i < 4; i++, tv += 1)
        {
          system.Update (tv);

          for (size_t j = 0, scenes_count = system.ScenesCount (); j < scenes_count; j++)
          {
            bound_volumes::aaboxf box = system.Scene (j).BoundBox ();

            printf ("  scene %d aabb(%.2f): [%.3f; %.3f; %.3f] - [%.3f; %.3f; %.3f]\n", j, tv.cast<float> (), box.minimum ().x, box.minimum ().y, box.minimum ().z,
                    box.maximum ().x, box.maximum ().y, box.maximum ().z);
          }
        }
      }
    }
    catch (std::exception& e)
    {
      printf ("exception: %s\n", e.what ());
    }
  }
}
