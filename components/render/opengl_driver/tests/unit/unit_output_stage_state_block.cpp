#include "shared.h"

int main ()
{
  printf ("Results of unit_output_stage_state_block_test:\n");
  
  try
  {
    Test test;
    
    IBlendState*        src_blend_state         = test.device->GetImmediateContext ()->OSGetBlendState ();
    IDepthStencilState* src_depth_stencil_state = test.device->GetImmediateContext ()->OSGetDepthStencilState ();
    IView*              src_render_target_view  = test.device->GetImmediateContext ()->OSGetRenderTargetView (0);
    IView*              src_depth_stencil_view  = test.device->GetImmediateContext ()->OSGetDepthStencilView ();

    StateBlockMask mask;        

    mask.Set (StateBlockGroup_OutputStage);
    
    mask.os_depth_stencil_view = false;
    
    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);
    
    state_block->Capture (test.device->GetImmediateContext ());
    
    test.device->GetImmediateContext ()->OSSetBlendState (0);
    test.device->GetImmediateContext ()->OSSetDepthStencilState (0);
    test.device->GetImmediateContext ()->OSSetRenderTargets (0, 0, 0);    
    
    printf ("after reset\n");
    
    IBlendState*        dst_blend_state         = test.device->GetImmediateContext ()->OSGetBlendState ();
    IDepthStencilState* dst_depth_stencil_state = test.device->GetImmediateContext ()->OSGetDepthStencilState ();    
    IView*              dst_render_target_view  = test.device->GetImmediateContext ()->OSGetRenderTargetView (0);
    IView*              dst_depth_stencil_view  = test.device->GetImmediateContext ()->OSGetDepthStencilView ();    
    
    printf ("OSGetBlendState:        %d\n", src_blend_state == dst_blend_state);
    printf ("OSGetDepthStencilState: %d\n", src_depth_stencil_state == dst_depth_stencil_state);
    printf ("OSGetRenderTargetView:  %d\n", src_render_target_view == dst_render_target_view);
    printf ("OSGetDepthStencilView:  %d\n", src_depth_stencil_view == dst_depth_stencil_view);    
    
    state_block->Apply (test.device->GetImmediateContext ());
    
    printf ("after apply\n");    
    
    dst_blend_state         = test.device->GetImmediateContext ()->OSGetBlendState ();
    dst_depth_stencil_state = test.device->GetImmediateContext ()->OSGetDepthStencilState ();    
    dst_render_target_view  = test.device->GetImmediateContext ()->OSGetRenderTargetView (0);
    dst_depth_stencil_view  = test.device->GetImmediateContext ()->OSGetDepthStencilView ();
    
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
