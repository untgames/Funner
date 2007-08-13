#include "shared.h"

typedef com_ptr<SpotLight> SpotLightPtr;

int main ()
{
  printf ("Results of light_spot_test:\n");
  
  SpotLightPtr light (SpotLight::Create (), false);

  light->SetAngle (30.4f);
  printf ("Spot angle: %f\n", light->Angle ());

  light->SetExponent (0.19f);
  printf ("Spot exponent: %f\n", light->Exponent ());

  return 0;
}
