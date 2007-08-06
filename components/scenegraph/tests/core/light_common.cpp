#include "shared.h"

typedef com_ptr<Light> LightPtr;

void dump_light_type (const LightPtr& light)
{
  switch (light->Type ())
  {
    case LightType_Direct: printf ("direct light");  break;
    case LightType_Spot:   printf ("spot light");    break;
    case LightType_Point:  printf ("point light");   break;
    default:               printf ("unknown light"); break;
  }
}

int main ()
{
  printf ("Results of light_common_test:\n");
  
  LightPtr light (Light::Create (), false);

  light->SetType (LightType_Spot);
  printf ("Light type: ");  
  dump_light_type (light);
  printf ("\n");
  light->SetType (LightType_Direct);
  printf ("Light type: ");  
  dump_light_type (light);
  printf ("\n");
  light->SetType (LightType_Point);
  printf ("Light type: ");  
  dump_light_type (light);
  printf ("\n");

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
