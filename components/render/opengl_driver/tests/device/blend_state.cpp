#include "shared.h"

int main ()
{
  printf ("Results of blend_state_test:\n");
  
  try
  {
//    Test test (L"OpenGL device test window (blend_state)", "disable='GL_EXT_blend_*' max_version=1.2");
    Test test (L"OpenGL device test window (blend_state)");
    
    dump_desc (test.device->GetImmediateContext ()->OSGetBlendState ());
    
    BlendDesc desc;
    
    memset (&desc, 0, sizeof (desc));

    desc.render_target [0].blend_enable                     = true;    
    desc.sample_alpha_to_coverage         = true;
    desc.render_target [0].blend_color_operation            = BlendOperation_Subtraction;
    desc.render_target [0].blend_color_source_argument      = BlendArgument_DestinationColor;
    desc.render_target [0].blend_color_destination_argument = BlendArgument_SourceAlpha;
    desc.render_target [0].blend_alpha_operation            = BlendOperation_Min;
    desc.render_target [0].blend_alpha_source_argument      = BlendArgument_One;
    desc.render_target [0].blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;    
    desc.render_target [0].color_write_mask                 = ColorWriteFlag_Red | ColorWriteFlag_Blue;

    BlendStatePtr state (test.device->CreateBlendState (desc), false);
    
    test.device->GetImmediateContext ()->OSSetBlendState (state.get ());
    
    dump_desc (test.device->GetImmediateContext ()->OSGetBlendState ());

    state = 0;

    dump_desc (test.device->GetImmediateContext ()->OSGetBlendState ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
