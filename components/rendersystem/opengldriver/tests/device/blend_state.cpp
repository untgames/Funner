#include "shared.h"

int main ()
{
  printf ("Results of blend_state_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (blend_state)");    
    
    dump_desc (*test.device->OSGetBlendState ());
    
    BlendDesc desc;
    
    memset (&desc, 0, sizeof (desc));

    desc.blend_enable                     = true;    
    desc.blend_color_operation            = BlendOperation_Subtraction;
    desc.blend_color_source_argument      = BlendArgument_DestinationColor;
    desc.blend_color_destination_argument = BlendArgument_SourceAlpha;
    desc.blend_alpha_operation            = BlendOperation_Min;
    desc.blend_alpha_source_argument      = BlendArgument_One;
    desc.blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;    
    desc.color_write_mask                 = ColorWriteFlag_Red | ColorWriteFlag_Blue;

    BlendStatePtr state (test.device->CreateBlendState (desc), false);
    
    dump_desc (*state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
