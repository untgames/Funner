#include "shared.h"

using namespace common;

int main ()
{
  printf ("Results of rasterizer_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (rasterizer_test)");

    RasterizerDesc desc;
    memset (&desc, 0, sizeof (desc));

    desc.fill_mode               = FillMode_Wireframe;
    desc.cull_mode               = CullMode_Front;
    desc.front_counter_clockwise = false;
    desc.depth_bias              = 1;
    desc.depth_clip_enable       = true;
    desc.scissor_enable          = true;
    desc.multisample_enable      = true;
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

    test.device->Draw (PrimitiveType_PointList, 0, 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
