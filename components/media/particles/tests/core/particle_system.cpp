#include "shared.h"

struct MyProcessor: IParticleProcessor, public xtl::reference_counter
{
  ~MyProcessor () { printf ("MyProcessor::~MyProcessor\n"); }

  void* AttachScene (ParticleScene&) { return 0; }
  void  DetachScene (ParticleScene&, void*) {}

  void AddRef () { addref (this); }
  void Release () { release (this); }

  void Process (ParticleScene& scene, const RandomGenerator&, void*)
  {
    for (ParticleList::Iterator it=scene.Particles ().CreateIterator (); it; ++it)
    {
      it->position_acceleration += math::vec3f (1.0f);
    }
  }
};

int main ()
{
  try
  {
    printf ("Results of particle_system:\n");

    xtl::intrusive_ptr<MyProcessor> processor (new MyProcessor, false);

    ParticleScene scene;

    scene.SetStartTimeOffset (1);

    ParticleList list = scene.Particles ();

    ParticleList::Iterator p = list.Add ();

    p->position_speed = math::vec3f (1.0f);
    p->lifetime       = 4.0f;

    p = list.Add ();

    p->position_speed = math::vec3f (-1.0f);
    p->lifetime       = 1.5f;

    ParticleSystem system;

    system.AddScene (scene);
    system.AttachProcessor (processor.get ());

    processor = 0;

    printf ("simulation:\n");
    TimeValue tv;
    
    for (size_t i=0; i<4; i++, tv += 1)
    {
      system.Update (tv);

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
