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
  printf ("Results of light_common_test:\n");
  
  PointLight::Pointer light (PointLight::Create ());

  light->SetLightColor (vec3f (0.f, 0.4f, 0.17f));  
  printf ("Light color: ");  
  dump (light->LightColor ());
  printf ("\n");
  
  light->SetIntensity (0.5f);  
  printf ("Light intensity: %.2f\n", light->Intensity ());  

  light->SetAttenuation (vec3f (0.f, 1.4f, -2.f));
  printf ("Light attenuation: ");  
  dump (light->Attenuation ());
  printf ("\n");
  
  light->SetRange (17.6f);
  printf ("Light range: %f\n", light->Range ());

  dump_bounds (*light);

  return 0;
}
