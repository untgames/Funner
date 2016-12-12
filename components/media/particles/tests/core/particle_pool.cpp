#include "shared.h"

struct MyParticle: public Particle
{
  int x [128];

  MyParticle () : x () {}
};

void initializer (Particle* p)
{
  new (p) MyParticle;
}

void generate (ParticleList& list, int count)
{
  for (int i=0; i<count; i++)
  	list.Add ();
}

void dump (const char* title, const ParticlePool& pool)
{
  printf ("%s: %u / %u\n", title, pool.Capacity (sizeof (Particle)), pool.Capacity (sizeof (MyParticle)));
}

int main ()
{
  try
  {
    printf ("Results of particle_pool:\n");
    
    ParticlePool pool;

    dump ("initial state", pool);

    ParticleList list1 (pool), list2 (pool, sizeof (MyParticle), &initializer), list3 (pool);

    generate (list1, 2000);
    generate (list2, 3000);
    generate (list3, 500);

    dump ("after adding particles", pool);

    list2.Clear ();
    pool.Shrink (0, list2.ParticleSize ());

    dump ("after list2.Clear & pool shrink", pool);

    list1.Clear ();
    pool.Shrink (0, list1.ParticleSize ());

    dump ("after list1.Clear & pool shrink", pool);

    list3.Clear ();
    pool.Shrink (1, list3.ParticleSize ());

    dump ("after list3.Clear & pool shrink", pool);

    pool.Reserve (300, list1.ParticleSize ());
    dump ("after reserve", pool);
  }
  catch (std::exception& e)
  {
  	printf ("%s\n", e.what ());
  }
}