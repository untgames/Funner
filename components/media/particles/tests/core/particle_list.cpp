#include "shared.h"

void dump (const ParticleList& list)
{
  printf ("list of %u items: {", list.Count ());

  const char* separator = "";

  for (ParticleList::ConstIterator it=list.CreateIterator (); it; ++it, separator = ", ")
    printf ("%s%u", separator, it->animation_frame);

  printf ("}\n");
}

int main ()
{
  try
  {
    printf ("Results of particle_list:\n");

    ParticleList list1;

    printf ("adding items\n");

    const unsigned int N = 10;

    for (unsigned int i=0; i<N; i++)
    {
      Particle& p = *list1.Add ();

      p.animation_frame = i;
    }

    dump (list1);

    printf ("removing items\n");

    list1.Remove (++list1.CreateIterator ());

    dump (list1);

    printf ("clearing items\n");

    list1.Clear ();

    dump (list1);
  }
  catch (std::exception& e)
  {
  	printf ("%s\n", e.what ());
  }
}