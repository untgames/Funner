#include "shared.h"

int main ()
{
  try
  {
    printf ("Results of particle_animation_frames:\n");

    ParticleScene scene;

    scene.SetAnimationFramesCount (4);
    scene.SetAnimationFramesPerSecond (1);

    ParticleList list = scene.Particles ();

    ParticleList::Iterator p = list.Add ();

    p->animation_frame_offset = 1;
    p->lifetime               = 40.0f;

    printf ("simulation:\n");
    TimeValue tv;
    RandomGenerator random;

    for (size_t i=0; i<6; i++, tv += 1)
    {
      scene.Update (tv, random);

      printf ("  time is %.3f: ", tv.cast<float> ());

      for (ParticleList::Iterator it=list.CreateIterator (); it; ++it)
        printf (" %u", p->animation_frame);

      printf ("\n");
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
