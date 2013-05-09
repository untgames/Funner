#ifndef RENDER_DX11_DRIVER_DEFAULT_RESOURCES_HEADER
#define RENDER_DX11_DRIVER_DEFAULT_RESOURCES_HEADER

#include <shared/common.h>
#include <shared/input_layout.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ресурсы по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DefaultResources
{
  InputLayoutPtr        input_layout;
  IProgramPtr           program;
  IBlendStatePtr        blend_state;
  IBlendStatePtr        null_blend_state;
  IDepthStencilStatePtr depth_stencil_state;
  IDepthStencilStatePtr null_depth_stencil_state;
  IRasterizerStatePtr   rasterizer_state;
  IRasterizerStatePtr   null_rasterizer_state;
};

}

}

}

#endif
