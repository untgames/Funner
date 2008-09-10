#include "shared.h"

struct State
{
  IRasterizerState* state;
  Viewport          viewport;
  Rect              scissor;
  
  void Init (IDevice& device)
  {
    state    = device.RSGetState ();
    viewport = device.RSGetViewport ();
    scissor  = device.RSGetScissor ();
  }
  
  static bool Check (const Viewport& v1, const Viewport& v2)
  {
    return Check ((const Rect&)v1, (const Rect&)v2) && v1.min_depth == v2.min_depth && v1.max_depth == v2.max_depth;
  }
  
  static bool Check (const Rect& r1, const Rect& r2)
  {
    return r1.x == r2.x && r1.y == r2.y && r1.width == r2.width && r1.height == r2.height;
  }

  void Check (const State& src)
  {
    printf ("ISGetState: %d\n", state == src.state);
    printf ("ISGetViewport: %d\n", Check (viewport, src.viewport));
    printf ("ISGetScissor: %d\n", Check (scissor, src.scissor));
  }
};

int main ()
{
  printf ("Results of unit_rasterizer_stage_state_block_test:\n");
  
  try
  {
    Test test;

    State src_state;

    src_state.Init (*test.device);

    StateBlockMask mask;        

    mask.Set (StateBlockGroup_RasterizerStage);    
    
    StateBlockPtr state_block (test.device->CreateStateBlock (mask), false);

    state_block->Capture ();

    Rect scissor;
    Viewport viewport;

    memset (&scissor, 0, sizeof scissor);
    memset (&viewport, 0, sizeof viewport);
    
    test.device->RSSetState (0);
    test.device->RSSetViewport (viewport);
    test.device->RSSetScissor  (scissor);    

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
