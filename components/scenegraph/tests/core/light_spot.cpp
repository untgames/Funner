#include "shared.h"

typedef com_ptr<Light> LightPtr;

int main ()
{
  printf ("Results of light_spot_test:\n");
  
  LightPtr light (Light::Create (), false);

  light->SetType (LightType_Spot);

  light->SetSpotAngle (30.4f);
  printf ("Spot angle: %f\n", light->SpotAngle ());

  light->SetSpotExponent (0.19f);
  printf ("Spot exponent: %f\n", light->SpotExponent ());

  return 0;
}
