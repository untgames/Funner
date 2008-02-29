#include "shared.h"

int main ()
{
  printf ("Results of output_stage_state_block_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (output_stage_state_block)");
    
    IBlendState*        src_blend_state         = test.device->OSGetBlendState ();
    IDepthStencilState* src_depth_stencil_state = test.device->OSGetDepthStencilState ();
    IView*              src_render_target_view  = test.device->OSGetRenderTargetView ();
    IView*              src_depth_stencil_view  = test.device->OSGetDepthStencilView ();

    StateBlockMask mask;        

    mask.Set (StateBlockGroup_OutputStage);
    
    mask.os_depth_stencil_view = false;
    
    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);
    
    state_block->Capture ();
    
    test.device->OSSetBlendState (0);
    test.device->OSSetDepthStencilState (0);
    test.device->OSSetRenderTargets (0, 0);    
    
    printf ("after reset\n");
    
    IBlendState*        dst_blend_state         = test.device->OSGetBlendState ();
    IDepthStencilState* dst_depth_stencil_state = test.device->OSGetDepthStencilState ();    
    IView*              dst_render_target_view  = test.device->OSGetRenderTargetView ();
    IView*              dst_depth_stencil_view  = test.device->OSGetDepthStencilView ();    
    
    printf ("OSGetBlendState:        %d\n", src_blend_state == dst_blend_state);
    printf ("OSGetDepthStencilState: %d\n", src_depth_stencil_state == dst_depth_stencil_state);
    printf ("OSGetRenderTargetView:  %d\n", src_render_target_view == dst_render_target_view);
    printf ("OSGetDepthStencilView:  %d\n", src_depth_stencil_view == dst_depth_stencil_view);    
    
    state_block->Apply ();
    
    printf ("after apply\n");    
    
    dst_blend_state         = test.device->OSGetBlendState ();
    dst_depth_stencil_state = test.device->OSGetDepthStencilState ();    
    dst_render_target_view  = test.device->OSGetRenderTargetView ();
    dst_depth_stencil_view  = test.device->OSGetDepthStencilView ();
    
    printf ("OSGetBlendState:        %d\n", src_blend_state == dst_blend_state);
    printf ("OSGetDepthStencilState: %d\n", src_depth_stencil_state == dst_depth_stencil_state);
    printf ("OSGetRenderTargetView:  %d\n", src_render_target_view == dst_render_target_view);
    printf ("OSGetDepthStencilView:  %d\n", src_depth_stencil_view == dst_depth_stencil_view);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
