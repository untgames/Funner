#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver::renderer2d;

CommonResources::CommonResources (render::low_level::IDevice* device)
{
  BlendDesc blend_desc;

  memset (&blend_desc, 0, sizeof (blend_desc));

  blend_desc.blend_enable             = false;
  blend_desc.sample_alpha_to_coverage = false;
  blend_desc.blend_color_operation    = BlendOperation_Add;
  blend_desc.blend_alpha_operation    = BlendOperation_Add;
  blend_desc.color_write_mask         = ColorWriteFlag_All;

  blend_states[BlendMode_None] = BlendStatePtr (device->CreateBlendState (blend_desc), false);

  blend_desc.blend_enable                     = true;
  blend_desc.blend_color_source_argument      = BlendArgument_SourceAlpha;
  blend_desc.blend_color_destination_argument = BlendArgument_InverseSourceAlpha;
  blend_desc.blend_alpha_source_argument      = BlendArgument_SourceAlpha;
  blend_desc.blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;
  
  blend_states[BlendMode_Translucent] = BlendStatePtr (device->CreateBlendState (blend_desc), false);

  blend_desc.blend_color_source_argument      = BlendArgument_Zero;
  blend_desc.blend_color_destination_argument = BlendArgument_SourceAlpha; //Should bi BlendArgument_SourceColor
  blend_desc.blend_alpha_source_argument      = BlendArgument_Zero;
  blend_desc.blend_alpha_destination_argument = BlendArgument_SourceAlpha;
  
  blend_states[BlendMode_Mask] = BlendStatePtr (device->CreateBlendState (blend_desc), false);

  blend_desc.blend_color_source_argument      = BlendArgument_One;
  blend_desc.blend_color_destination_argument = BlendArgument_One;
  blend_desc.blend_alpha_source_argument      = BlendArgument_One;
  blend_desc.blend_alpha_destination_argument = BlendArgument_One;
  
  blend_states[BlendMode_Additive] = BlendStatePtr (device->CreateBlendState (blend_desc), false);

  BufferDesc constant_buffer_desc;

  memset (&constant_buffer_desc, 0, sizeof (constant_buffer_desc));

  constant_buffer_desc.size         = sizeof (ProgramParameters);
  constant_buffer_desc.usage_mode   = UsageMode_Default;
  constant_buffer_desc.bind_flags   = BindFlag_ConstantBuffer;
  constant_buffer_desc.access_flags = AccessFlag_ReadWrite;

  constant_buffer = BufferPtr (device->CreateBuffer (constant_buffer_desc), false);

  device->SSSetConstantBuffer (0, constant_buffer.get ());

  DepthStencilDesc depth_stencil_desc;

  memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

  depth_stencil_desc.depth_test_enable   = true;
  depth_stencil_desc.stencil_test_enable = false;
  depth_stencil_desc.depth_compare_mode  = CompareMode_Less;
  depth_stencil_desc.depth_write_enable  = false;

  depth_stencil_states[0] = DepthStencilStatePtr (device->CreateDepthStencilState (depth_stencil_desc), false);

  depth_stencil_desc.depth_write_enable = true;

  depth_stencil_states[1] = DepthStencilStatePtr (device->CreateDepthStencilState (depth_stencil_desc), false);
}

render::low_level::IDepthStencilState* CommonResources::GetDepthStencilState (bool depth_write_enabled) 
{ 
  if (depth_write_enabled) return depth_stencil_states[1].get (); 
  else                     return depth_stencil_states[0].get (); 
}
