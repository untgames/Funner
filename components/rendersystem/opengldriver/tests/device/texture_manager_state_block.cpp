#include "shared.h"

struct State
{
  ISamplerState* samplers [DEVICE_SAMPLER_SLOTS_COUNT];
  ITexture*      textures [DEVICE_SAMPLER_SLOTS_COUNT];
  
  void Init (IDevice& device)
  {
    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      samplers [i] = device.SSGetSampler (i);
      textures [i] = device.SSGetTexture (i);
    }
  }  

  void Check (const State& src)
  {
    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      printf ("SSGetTexture(%u): %d\n", i, textures [i] == src.textures [i]);
      
    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      printf ("SSGetSamplerState(%u): %d\n", i, samplers [i] == src.samplers [i]);      
  }
};

int main ()
{
  printf ("Results of texture_manager_state_block_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture_manager_state_block)");
    
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
    sampler_desc.max_anisotropy       = 0;
    sampler_desc.mag_filter           = TexMagFilter_Linear;
    sampler_desc.wrap_u               = TexcoordWrap_Clamp;
    sampler_desc.wrap_v               = TexcoordWrap_Clamp;
    sampler_desc.wrap_w               = TexcoordWrap_Clamp;
    sampler_desc.comparision_function = CompareMode_Less;
    sampler_desc.mip_lod_bias         = 0.2f;
    sampler_desc.min_lod              = 1.f;
    sampler_desc.max_lod              = 2.f;    

    TexturePtr texture (test.device->CreateTexture (texture_desc), false);
    SamplerStatePtr sampler (test.device->CreateSamplerState (sampler_desc), false);        
    
    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      test.device->SSSetSampler (i, sampler.get ());
      test.device->SSSetTexture (i, texture.get ());
    }    

    State src_state;

    src_state.Init (*test.device);

    StateBlockMask mask;        

    mask.Set (StateBlockGroup_ShaderStage);
    
    mask.ss_program = false;
    mask.ss_program_parameters_layout = false;
    
    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      mask.ss_constant_buffers [i] = false;
    
    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);

    state_block->Capture ();
        
    for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    {
      test.device->SSSetSampler (i, 0);
      test.device->SSSetTexture (i, 0);
    }

    printf ("after reset\n");
    
    State dst_state;
    
    dst_state.Init (*test.device);    
    dst_state.Check (src_state);

    state_block->Apply ();

    printf ("after apply\n");    

    dst_state.Init (*test.device);
    dst_state.Check (src_state);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
