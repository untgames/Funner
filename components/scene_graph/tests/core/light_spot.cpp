#include "shared.h"

void dump (const aaboxf& box)
{
  printf ("[");
  dump (box.minimum ());
  printf (" - ");
  dump (box.maximum ());  
  printf ("]");
}

void dump_bounds (Light& light)
{
  if (light.IsInfiniteBounds ()) printf ("light '%s' has infinite bounds:\n  local=", light.Name ());
  else                           printf ("light '%s' bound:\n  local=", light.Name ());

  dump (light.BoundBox ());
  printf ("\n  world=");
  dump (light.WorldBoundBox ());
  printf ("\n");
}

int main ()
{
  printf ("Results of light_spot_test:\n");
  
  SpotLight::Pointer light (SpotLight::Create ());

  light->SetAngle (30.4f);
  printf ("Spot angle: %f\n", light->Angle ());

  light->SetExponent (0.19f);
  printf ("Spot exponent: %f\n", light->Exponent ());

  light->SetRange (10.f);

  dump_bounds (*light);

  return 0;
}
