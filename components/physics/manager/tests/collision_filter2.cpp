#include "shared.h"

bool filter (const RigidBody& body1, const RigidBody& body2, const char* mask1, const char* mask2)
{
  printf ("filter registered for masks %s %s called for bodies from group %s and %s\n", mask1, mask2, body1.CollisionGroup (), body2.CollisionGroup ());

  return true;
}

void collision_event_handler (RigidBody& body1, RigidBody& body2, const char* mask1, const char* mask2)
{
  printf ("handler registered for masks %s %s called for bodies from group %s and %s\n", mask1, mask2 ? mask2 : "-", body1.CollisionGroup (), body2.CollisionGroup ());
}

int main ()
{
  printf ("Results of collision_filter2_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    const char* filters [] = { "ground", "animal.dog",
                               "animal.dog", "plant.tree*",
                               "animal*", "plant*",
                               "ground", "*"};
    const char* handlers [] = { "ground", "*",
                                "animal*", "plant*",
                                "animal.dog", "plant.tree*"};

    for (size_t i = 0, count = sizeof (filters) / sizeof (*filters); i < count; i += 2)
      scene.AddCollisionFilter (filters [i], filters [i + 1], true, xtl::bind (filter, _1, _2, filters [i], filters [i + 1]));

    for (size_t i = 0, count = sizeof (handlers) / sizeof (*handlers); i < count; i += 2)
      scene.RegisterCollisionCallback (handlers [i], handlers [i + 1], CollisionEventType_Process, xtl::bind (collision_event_handler, _2, _3, handlers [i], handlers [i + 1]));

    const char* groups [] = { "ground", "animal", "animal.cat", "animal.dog", "plant", "plant.tree", "plant.tree.oak", "plant.tree.apple", "plant.flower" };
    const char* body_handlers [] = { "animal*", 0, "plant.flower", "plant.tree*", "*" };

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    stl::vector<RigidBody> bodies;

    for (size_t i = 0, count = sizeof (groups) / sizeof (*groups); i < count; i++)
    {
      RigidBody body (scene.CreateRigidBody (sphere_shape, 1));

      body.SetCollisionGroup (groups [i]);

      bodies.push_back (body);
    }

    for (size_t i = 0, count = sizeof (body_handlers) / sizeof (*body_handlers); i < count; i++)
    {
      if (body_handlers [i])
        bodies [i].RegisterCollisionCallback (body_handlers [i], CollisionEventType_Process, xtl::bind (collision_event_handler, _2, _3, body_handlers [i], (const char*)0));
    }

    scene.PerformSimulation (1.f);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
