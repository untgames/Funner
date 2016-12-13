#include "shared.h"

int main ()
{
  try
  {
    printf ("Results of particle_scene:\n");

    ParticleScene scene;

    scene.SetName ("scene1");
    scene.SetMaterialName ("material1");
    scene.SetAnimationFramesCount (2);
    scene.SetStartTimeOffset (1);

    printf ("name='%s'\n", scene.Name ());
    printf ("material='%s'\n", scene.MaterialName ());
    printf ("offset=[%.3f; %.3f; %.3f]\n", scene.Offset ().x, scene.Offset ().y, scene.Offset ().z);
    printf ("time_offset=%.2f\n", scene.StartTimeOffset ().cast<float> ());
    printf ("animation_frames=%u\n", scene.AnimationFramesCount ());

    for (size_t i=0; i<scene.AnimationFramesCount (); i++)
    {
      const ParticleTexDesc& desc = scene.AnimationFrames ()[i];

      printf ("  offset=[%.3f; %.3f] size=[%.3f; %.3f]\n", desc.tex_offset.x, desc.tex_offset.y, desc.tex_size.x, desc.tex_size.y);
    }

    ParticleList list = scene.Particles ();

    ParticleList::Iterator p = list.Add ();

    p->position_speed = math::vec3f (1.0f);
    p->lifetime       = 4.0f;

    p = list.Add ();

    p->position_speed = math::vec3f (-1.0f);
    p->lifetime       = 2.5f;

    printf ("simulation:\n");
    TimeValue tv;
    RandomGenerator random;

    for (size_t i=0; i<4; i++, tv += 1)
    {
      scene.Update (tv, random);

      bound_volumes::aaboxf box = scene.BoundBox ();

      printf ("  aabb(%.2f): [%.3f; %.3f; %.3f] - [%.3f; %.3f; %.3f]\n", tv.cast<float> (), box.minimum ().x, box.minimum ().y, box.minimum ().z,
        box.maximum ().x, box.maximum ().y, box.maximum ().z);
    }
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
