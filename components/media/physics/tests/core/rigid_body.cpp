#include "shared.h"

using namespace media::physics;

void dump (const RigidBody& body)
{
  printf ("Body '%s':\n", body.Name ());
  printf ("  mass                       %.2f\n", body.Mass ());
  printf ("  mass space inertia tensor  "); dump (body.MassSpaceInertiaTensor ());  printf ("\n");
  printf ("  flags                      %08x\n", body.Flags ());
  printf ("  shape                      '%s'\n", body.Shape ().Name ());
  printf ("  material                   '%s'\n", body.Material ().Name ());
}

int main ()
{
  printf ("Results of rigid_body_test:\n");

  try
  {
    RigidBody body, clonned_body;

    body.Rename ("default body");
    dump (body);
    clonned_body = body.Clone ();
    dump (clonned_body);

    Material material;

    material.Rename ("body material");

    Shape shape;

    shape.Rename ("body shape");

    body.SetMass (10.f);
    body.SetMassSpaceInertiaTensor (math::vec3f (5.f, 6.f, 7.f));
    body.SetShape (shape);
    body.SetMaterial (material);
    body.SetFlags (RigidBodyFlag_FrozenRotation);

    body.Rename ("modified body");
    dump (body);
    clonned_body = body.Clone ();
    dump (clonned_body);
  }
  catch (xtl::exception& e)
  {
    printf ("exception '%s'\n", e.what ());
  }

  return 0;
}
