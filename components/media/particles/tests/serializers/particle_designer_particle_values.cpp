#include "shared.h"

const char* LIBRARY_NAMES [] = { "data/particle_designer/gravity_test.plist", "data/particle_designer/radius_test.plist" };

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of particle_designer_particle_values_test:\n");

  LogFilter log_filter ("*", &print_log);

  for (size_t current_library = 0, count = sizeof (LIBRARY_NAMES) / sizeof (*LIBRARY_NAMES); current_library < count; current_library++)
  {
    printf ("\nTesting library '%s':\n", LIBRARY_NAMES [current_library]);

    try
    {
      ParticleSystemLibrary library (LIBRARY_NAMES [current_library]);

      MyRand rand_function;

      RandomGenerator random (rand_function);

      for (ParticleSystemLibrary::Iterator iter = library.CreateIterator (); iter; ++iter)
      {
        ParticleSystem system (random);

        (*iter)->Configure (system);

        ParticleScene&       scene     = system.Scene (0);
        const ParticleList&  particles = scene.Particles();

        TimeValue tv (20, 60); //skip first 1/3 seconds to generate particles

        system.Update (tv);

        for (size_t i = 0; i < 100; i++, tv += TimeValue (1, 60))
        {
          system.Update (tv);

          bound_volumes::aaboxf box = scene.BoundBox ();

          printf ("  time: %.2f\n", tv.cast<float> ());
          printf ("  aabb: [%.3f; %.3f; %.3f] - [%.3f; %.3f; %.3f]\n", box.minimum ().x, box.minimum ().y, box.minimum ().z, box.maximum ().x, box.maximum ().y, box.maximum ().z);
          printf ("  particles %u:\n", particles.Count ());

          for (ParticleList::ConstIterator particle = particles.CreateIterator (); particle; ++particle)
          {
            printf ("    particle:\n");
            printf ("      position: %.3f %.3f %.3f\n", particle->position.x, particle->position.y, particle->position.z);
            printf ("      rotation: %.3f\n", math::degree (particle->rotation));
            printf ("      size: %.3f %.3f\n", particle->size.x, particle->size.y);
            printf ("      color: %.3f %.3f %.3f %.3f\n", particle->color.x, particle->color.y, particle->color.z, particle->color.w);
            printf ("      lifetime: %.3f\n", particle->lifetime);
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
