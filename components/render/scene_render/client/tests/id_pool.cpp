#include "shared.h"

int main ()
{
  printf ("Results of id_pool_test:\n");

  try
  {
    IdPool pool;

    static const size_t PROBES_COUNT = 10000;

    stl::vector<size_t> v;

    for (size_t i=0; i<PROBES_COUNT; i++)
    {
      v.push_back (pool.Allocate ());
    }

    while (!v.empty ())
    {
      pool.Deallocate (v.back ());

      v.pop_back ();
    }

    size_t id = 0;

    printf ("test id is %u\n", id = pool.Allocate ());

    pool.Deallocate (id);

    for (size_t i=0; i<PROBES_COUNT; i++)
    {
      v.push_back (pool.Allocate ());
    }

    for (size_t i=PROBES_COUNT/2; i<PROBES_COUNT; i++)
    {
printf ("..%u\n", v [i]);

      pool.Deallocate (v [i]);
    }
  
    v.clear ();

    printf ("test id is %u\n", id = pool.Allocate ());

    pool.Deallocate (id);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
