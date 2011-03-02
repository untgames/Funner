#include "shared.h"

using namespace media::physics;

void dump (const Material& material)
{
  printf ("Material '%s':\n", material.Name ());
  printf ("  linear damping       %.2f\n", material.LinearDamping ());
  printf ("  angular damping      %.2f\n", material.AngularDamping ());
  printf ("  friction             %.2f\n", material.Friction ());
  printf ("  anisotropic friction ");
  dump (material.AnisotropicFriction ());
  printf ("\n");
  printf ("  restitution          %.2f\n", material.Restitution ());
}

int main ()
{
  printf ("Results of material_test:\n");

  try
  {
    Material material;

    material.Rename ("default material");
    dump (material);

    material.SetLinearDamping       (0.1f);
    material.SetAngularDamping      (0.1f);
    material.SetFriction            (0.1f);
    material.SetAnisotropicFriction (math::vec3f (0.1f, 0.2f, 0.3f));
    material.SetRestitution         (0.1f);

    material.Rename ("modified material");
    dump (material);
    dump (material.Clone ());
  }
  catch (xtl::exception& e)
  {
    printf ("exception '%s'\n", e.what ());
  }

  return 0;
}
