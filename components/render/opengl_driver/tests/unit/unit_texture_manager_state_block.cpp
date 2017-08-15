#include "shared.h"

struct State
{
  ISamplerState* samplers [DEVICE_SAMPLER_SLOTS_COUNT];
  ITexture*      textures [DEVICE_SAMPLER_SLOTS_COUNT];
  
  void Init (IDevice& device)
  {
    for (unsigned int i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      samplers [i] = device.GetImmediateContext ()->SSGetSampler (i);
      textures [i] = device.GetImmediateContext ()->SSGetTexture (i);
    }
  }  

  void Check (const State& src)
  {
    for (unsigned int i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      printf ("SSGetTexture(%u): %d\n", i, textures [i] == src.textures [i]);
      
    for (unsigned int i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      printf ("SSGetSamplerState(%u): %d\n", i, samplers [i] == src.samplers [i]);      
  }
};

void on_application_initialized ()
{
  try
  {
    Test test;

    TextureDesc texture_desc;
    SamplerDesc sampler_desc;

    memset (&texture_desc, 0, sizeof (texture_desc));
    memset (&sampler_desc, 0, sizeof (sampler_desc));

    texture_desc.dimension            = TextureDimension_2D;
    texture_desc.width                = 32;
    texture_desc.height               = 32;
    texture_desc.layers               = 1;
    texture_desc.format               = PixelFormat_RGB8;
    texture_desc.bind_flags           = BindFlag_Texture;
    texture_desc.generate_mips_enable = false;
    texture_desc.access_flags         = AccessFlag_ReadWrite;

    sampler_desc.min_filter           = TexMinFilter_LinearMipLinear;
    sampler_desc.max_anisotropy       = 1;
    sampler_desc.mag_filter           = TexMagFilter_Linear;
    sampler_desc.wrap_u               = TexcoordWrap_Clamp;
    sampler_desc.wrap_v               = TexcoordWrap_Clamp;
    sampler_desc.wrap_w               = TexcoordWrap_Repeat;
    sampler_desc.comparision_function = CompareMode_AlwaysPass;
    sampler_desc.mip_lod_bias         = 0.0f;
    sampler_desc.min_lod              = 0.0f;
    sampler_desc.max_lod              = FLT_MAX;

    TexturePtr texture (test.device->CreateTexture (texture_desc), false);
    SamplerStatePtr sampler (test.device->CreateSamplerState (sampler_desc), false);

    test.device->GetImmediateContext ()->SSSetSampler (0, sampler.get ());
    test.device->GetImmediateContext ()->SSSetTexture (0, texture.get ());

    State src_state;

    src_state.Init (*test.device);

    StateBlockMask mask;

    mask.Set (StateBlockGroup_ShaderStage);

    mask.ss_program = false;
    mask.ss_program_parameters_layout = false;

    for (unsigned int i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      mask.ss_constant_buffers [i] = false;

    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);

    state_block->Capture (test.device->GetImmediateContext ());

    test.device->GetImmediateContext ()->SSSetSampler (0, 0);
    test.device->GetImmediateContext ()->SSSetTexture (0, 0);

    printf ("after reset\n");

    State dst_state;

    dst_state.Init (*test.device);
    dst_state.Check (src_state);

    state_block->Apply (test.device->GetImmediateContext ());

    printf ("after apply\n");

    dst_state.Init (*test.device);
    dst_state.Check (src_state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of unit_texture_manager_state_block_test:\n");
  
  try
  {
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, &on_application_initialized);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
