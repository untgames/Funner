#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

RasterizerState::RasterizerState (const DeviceManager& manager, const RasterizerDesc& in_desc)
  : DeviceObject (manager)
  , desc (in_desc)
{
  try
  {
      //преобразование дескриптора

    D3D11_RASTERIZER_DESC dx_desc;

    memset (&dx_desc, 0, sizeof (dx_desc));

    dx_desc.FrontCounterClockwise = desc.front_counter_clockwise;
    dx_desc.DepthBias             = desc.depth_bias;
    dx_desc.DepthBiasClamp        = 0.0f;
    dx_desc.DepthClipEnable       = TRUE;
    dx_desc.SlopeScaledDepthBias  = 0.0f;
    dx_desc.MultisampleEnable     = desc.multisample_enable;
    dx_desc.ScissorEnable         = desc.scissor_enable;
    dx_desc.AntialiasedLineEnable = desc.antialiased_line_enable;

    switch (desc.fill_mode)
    {
      case FillMode_Wireframe:
        dx_desc.FillMode = D3D11_FILL_WIREFRAME;
        break;
      case FillMode_Solid:
        dx_desc.FillMode = D3D11_FILL_SOLID;
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.fill_mode", desc.fill_mode);
    }

    switch (desc.cull_mode)
    {
      case CullMode_None:
        dx_desc.CullMode = D3D11_CULL_NONE;
        break;
      case CullMode_Front:
        dx_desc.CullMode = D3D11_CULL_FRONT;
        break;
      case CullMode_Back:
        dx_desc.CullMode = D3D11_CULL_BACK;
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.cull_mode", desc.cull_mode);
    }

      //создание состояния

    ID3D11RasterizerState* dx_state = 0;

    check_errors ("ID3D11Device::CreateRasterizerState", manager.GetDevice ().CreateRasterizerState (&dx_desc, &dx_state));

    if (!dx_state)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateRasterizerState failed");

    state = DxRasterizerStatePtr (dx_state, false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::RasterizerState::RasterizerState");
    throw;
  }
}

RasterizerState::~RasterizerState ()
{
}

/*
    Получение дескриптора
*/

void RasterizerState::GetDesc (RasterizerDesc& out_desc)
{
  out_desc = desc;
}
