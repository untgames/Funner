#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver::renderer2d;

CommonResources::CommonResources (render::low_level::IDevice* device)
{
  BufferDesc constant_buffer_desc;

  memset (&constant_buffer_desc, 0, sizeof (constant_buffer_desc));

  constant_buffer_desc.size         = sizeof (ProgramParameters);
  constant_buffer_desc.usage_mode   = UsageMode_Default;
  constant_buffer_desc.bind_flags   = BindFlag_ConstantBuffer;
  constant_buffer_desc.access_flags = AccessFlag_ReadWrite;

  constant_buffer = BufferPtr (device->CreateBuffer (constant_buffer_desc), false);

  device->SSSetConstantBuffer (0, constant_buffer.get ());

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
}