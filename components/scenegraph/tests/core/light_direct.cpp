#include "shared.h"

typedef com_ptr<DirectLight> DirectLightPtr;

int main ()
{
  printf ("Results of light_direct_test:\n");
  
  DirectLightPtr light (DirectLight::Create (), false);

  light->SetRadius (48.5f);
  printf ("Light radius: %f\n", light->Radius ());

  return 0;
}
