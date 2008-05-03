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
  if (light.IsInfiniteBounds ()) printf ("light has infinite bounds:\n  local=", light.Name ());
  else                           printf ("light bound:\n  local=", light.Name ());

  dump (light.BoundBox ());
  printf ("\n  world=");
  dump (light.WorldBoundBox ());
  printf ("\n");
}

int main ()
{
  printf ("Results of light_direct_test:\n");
  
  DirectLight::Pointer light (DirectLight::Create ());

  light->SetRadius (48.5f);
  printf ("Light radius: %f\n", light->Radius ());

  light->SetRange (10.f);

  dump_bounds (*light);

  return 0;
}
