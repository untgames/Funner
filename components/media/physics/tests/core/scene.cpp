#include "shared.h"

using namespace media::physics;

void dump (const Scene& scene)
{
  printf ("Scene '%s':\n", scene.Name ());
  printf ("  gravity                 ");
  dump (scene.Gravity ());
  printf ("\n  simulation step       %.3f\n", scene.SimulationStep ());
  printf ("  collision filters count %u\n", scene.CollisionFilters ().Size ());

  if (scene.CollisionFilters ().Size ())
    printf ("  collision filters:\n");
  for (Scene::CollisionFilterCollection::ConstIterator i = scene.CollisionFilters ().CreateIterator (); i; ++i)
    printf ("    collision filter '%s' with '%s' collides %d\n", i->Group1Wildcard (), i->Group2Wildcard (), i->IsCollides ());
}

int main ()
{
  printf ("Results of scene_test:\n");

  try
  {
    Scene scene;

    scene.Rename ("default scene");
    dump (scene);

    scene.SetGravity        (math::vec3f (0.1f, 0.2f, 0.3f));
    scene.SetSimulationStep (0.1f);

    Scene::CollisionFilterCollection filter_collection = scene.CollisionFilters ();

    const char *collision_filters_groups1 [] = { "group1_1", "group1_2", "group1_3", "group1_4" },
               *collision_filters_groups2 [] = { "group2_1", "group2_2", "group2_3", "group2_4" };
    bool       collision_filters []          = { true, true, false, true };

    for (size_t i = 0; i < 4; i++)
    {
      CollisionFilter filter;

      filter.SetGroup1Wildcard (collision_filters_groups1 [i]);
      filter.SetGroup2Wildcard (collision_filters_groups2 [i]);
      filter.SetCollides (collision_filters [i]);

      filter_collection.Add (filter);
    }

    scene.Rename ("modified scene");
    dump (scene);
    dump (scene.Clone ());
  }
  catch (xtl::exception& e)
  {
    printf ("exception '%s'\n", e.what ());
  }

  return 0;
}
