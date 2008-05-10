#include "shared.h"

int main ()
{
  printf ("Results of material2_test:\n");
  
  TestMaterial::Pointer material = TestMaterial::Create ();
  
  material->Rename ("Material1");
  material->Enable (MaterialPin_TwoSided);
  material->Enable (MaterialPin_Wireframe);
//  material->Enable (MaterialPin_Lighting);
  material->Enable (MaterialPin_CastShadows);
//  material->Enable (MaterialPin_ReceiveShadows);
  material->Enable (MaterialPin_SelfShadow);
  
  printf ("Material '%s'\n", material->Name ());
  
  static const char* pin_name [] = {"two_sided", "wireframe", "lighting", "cast_shadows", "receive_shadows",
                                    "self_shadow"};

  for (size_t i=0; i<MaterialPin_Num; i++)
    printf ("  pin '%s' %s\n", pin_name [i], material->IsEnabled ((MaterialPin)i) ? "enabled" : "disabled");

  return 0;
}
