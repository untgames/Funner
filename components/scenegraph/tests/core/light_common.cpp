#include "shared.h"

typedef com_ptr<PointLight> PointLightPtr;

int main ()
{
  printf ("Results of light_common_test:\n");
  
  PointLightPtr light (PointLight::Create (), false);

  light->SetLightColor (vec3f (0.f, 0.4f, 0.17f));
  printf ("Light color: ");  
  dump (light->LightColor ());
  printf ("\n");

  light->SetAttenuation (vec3f (0.f, 1.4f, -2.f));
  printf ("Light attenuation: ");  
  dump (light->Attenuation ());
  printf ("\n");
  
  light->SetRange (17.6f);
  printf ("Light range: %f\n", light->Range ());

  return 0;
}
