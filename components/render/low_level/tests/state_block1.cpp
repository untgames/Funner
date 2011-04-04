#include "shared.h"

void dump (const char* name, const StateBlockMask& mask)
{
  printf ("Mask '%s' (hash=%08x):\n", name, mask.Hash ());
  printf ("ISInpoutLayout: %d\n", mask.is_layout);
  
  for (size_t i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
    printf ("ISVertexBuffers[%u]: %d\n", i, mask.is_vertex_buffers [i]);
    
  printf ("ISIndexBuffer: %d\n", mask.is_index_buffer);

  printf ("SSProgram: %d\n", mask.ss_program);
  
  for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
    printf ("SSConstantBuffers[%u]: %d\n", i, mask.ss_constant_buffers [i]);  
    
  printf ("SSProgramParametersLayout: %d\n", mask.ss_program_parameters_layout);
  
  for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    printf ("SSSamplers[%u]: %d\n", i, mask.ss_samplers [i]);
    
  for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
    printf ("SSTextures[%u]: %d\n", i, mask.ss_samplers [i]);      
    
  printf ("RSState: %d\n", mask.rs_state);
  printf ("RSViewport: %d\n", mask.rs_viewport);
  printf ("RSScissor: %d\n", mask.rs_scissor);
  printf ("OSBlendState: %d\n", mask.os_blend_state);
  printf ("OSDepthStencilState: %d\n", mask.os_depth_stencil_state);
  printf ("OSRenderTargetView: %d\n", mask.os_render_target_view);
  printf ("OSDepthStencilView: %d\n", mask.os_depth_stencil_view);
  printf ("Predication: %d\n", mask.predication);
}

int main ()
{
  printf ("Results of state_block_test:\n");
  
  StateBlockMask mask1;
  
  mask1.Set (StateBlockGroup_RasterizerStage);  
  
  StateBlockMask mask2 = ~mask1;    
  
  mask2.Clear (StateBlockGroup_ShaderStage);
  
  StateBlockMask mask3 = mask1 ^ mask2;
  
  dump ("mask1", mask1);
  dump ("mask2", mask2);
  dump ("mask3", mask3);

  StateBlockMask mask4 = mask3;

  printf ("mask3 == mask4: %d\n", mask3 == mask4);

  return 0;
}
