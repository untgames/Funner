#include "shared.h"

using namespace common;

//печать дескриптора rasterizer-state
inline void dump_desc (IRasterizerState& state)
{
  if (!&state)
  {
    printf ("Null rasterizer state\n");
    
    return;
  }

  using render::low_level::get_name;

  RasterizerDesc desc;
  
  state.GetDesc (desc);
  
  printf ("Rasterizer state:\n");
  printf ("  fill_mode:               %s\n", get_name (desc.fill_mode));
  printf ("  cull_mode:               %s\n", get_name (desc.cull_mode));
  printf ("  front_counter_clockwise: %s\n", desc.front_counter_clockwise ? "true" : "false");
  printf ("  depth_bias:              %d\n", desc.depth_bias);
  printf ("  scissor_enable:          %s\n", desc.scissor_enable ? "true" : "false");
  printf ("  multisample_enable:      %s\n", desc.multisample_enable ? "true" : "false");  
  printf ("  antialiased_line_enable: %s\n", desc.antialiased_line_enable ? "true" : "false");
}

int main ()
{
  printf ("Results of unit_rasterizer_test:\n");
  
  try
  {
    Test test;

    dump_desc (*test.device->RSGetState ());    

    RasterizerDesc desc;
    memset (&desc, 0, sizeof (desc));

    desc.fill_mode               = FillMode_Wireframe;
    desc.cull_mode               = CullMode_Front;
    desc.front_counter_clockwise = false;
    desc.depth_bias              = 1;
    desc.scissor_enable          = true;
    desc.multisample_enable      = false;
    desc.antialiased_line_enable = false;

    xtl::com_ptr<IRasterizerState> rasterizer (test.device->CreateRasterizerState (desc), false);

    Rect scissor = {0, 0, 128, 128};
    Viewport viewport;
    
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = 128;
    viewport.height = 128;
    viewport.min_depth = 0.1f;
    viewport.max_depth = 10.9f;

    test.device->RSSetState    (rasterizer.get ());
    test.device->RSSetViewport (viewport);
    test.device->RSSetScissor  (scissor);

    dump_desc (*test.device->RSGetState ());    

    test.device->Draw (PrimitiveType_PointList, 0, 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
