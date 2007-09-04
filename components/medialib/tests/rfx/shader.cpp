#include "shared.h"

int main ()
{
  printf ("Results of shader_test:\n");
  
  TestShader::Pointer shader = TestShader::Create ();
  
  shader->Rename ("shader1");
  shader->Enable (ShaderPin_TwoSided);
  shader->Enable (ShaderPin_Wireframe);
//  shader->Enable (ShaderPin_Lighting);
  shader->Enable (ShaderPin_CastShadows);
//  shader->Enable (ShaderPin_ReceiveShadows);
  shader->Enable (ShaderPin_SelfShadow);
  
  printf ("Shader '%s'\n", shader->Name ());
  
  static const char* pin_name [] = {"two_sided", "wireframe", "lighting", "cast_shadows", "receive_shadows",
                                    "self_shadow"};

  for (size_t i=0; i<ShaderPin_Num; i++)
    printf ("  pin '%s' %s\n", pin_name [i], shader->IsEnabled ((ShaderPin)i) ? "enabled" : "disabled");

  return 0;
}
