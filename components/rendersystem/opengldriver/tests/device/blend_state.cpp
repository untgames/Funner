#include "shared.h"

int main ()
{
  printf ("Results of blend_state_test:\n");
  
  try
  {
    Test test ("OpenGL device test window (blend_state)");    
    
    dump_desc (*test.device->OSGetBlendState ());
    
    BlendDesc desc;
    
    memset (&desc, 0, sizeof (desc));

    desc.blend_enable               = true;    
    desc.blend_operation            = BlendOperation_Subtraction;
    desc.blend_source_argument      = BlendArgument_SourceAlpha;
    desc.blend_destination_argument = BlendArgument_InverseSourceAlpha;
    desc.color_write_mask           = ColorWriteFlag_Red | ColorWriteFlag_Blue;

    IBlendState* state = test.device->CreateBlendState (desc);
    
    dump_desc (*state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
