#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Утилиты
*/

namespace
{

D3D11_COMPARISON_FUNC convert (CompareMode mode, const char* param)
{
  switch (mode)
  {
    case CompareMode_AlwaysFail:   return D3D11_COMPARISON_NEVER;
    case CompareMode_AlwaysPass:   return D3D11_COMPARISON_ALWAYS;
    case CompareMode_Equal:        return D3D11_COMPARISON_EQUAL;
    case CompareMode_NotEqual:     return D3D11_COMPARISON_NOT_EQUAL;
    case CompareMode_Less:         return D3D11_COMPARISON_LESS;
    case CompareMode_LessEqual:    return D3D11_COMPARISON_LESS_EQUAL;
    case CompareMode_Greater:      return D3D11_COMPARISON_GREATER;
    case CompareMode_GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
    default:                       throw xtl::make_argument_exception ("", param, mode);
  }
}

D3D11_STENCIL_OP convert (StencilOperation op, const char* param)
{
  switch (op)
  {
    case StencilOperation_Keep:      return D3D11_STENCIL_OP_KEEP;
    case StencilOperation_Zero:      return D3D11_STENCIL_OP_ZERO;
    case StencilOperation_Replace:   return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation_Increment: return D3D11_STENCIL_OP_INCR;
    case StencilOperation_Decrement: return D3D11_STENCIL_OP_DECR;
    case StencilOperation_Invert:    return D3D11_STENCIL_OP_INVERT;
    default:                         throw xtl::make_argument_exception ("", param, op);
  }
}

void convert (const StencilDesc& desc, D3D11_DEPTH_STENCILOP_DESC& dx_desc, const char* prefix)
{
  try
  {
    dx_desc.StencilFailOp      = convert (desc.stencil_fail_operation, "desc.stencil_fail_operation");
    dx_desc.StencilDepthFailOp = convert (desc.depth_fail_operation, "desc.depth_fail_operation");
    dx_desc.StencilPassOp      = convert (desc.stencil_pass_operation, "desc.stencil_pass_operation");
    dx_desc.StencilFunc        = convert (desc.stencil_compare_mode, "desc.stencil_compare_mode");
  }
  catch (xtl::exception& e)
  {
    e.touch ("processing %s", prefix);
    throw;
  }
}

}

/*
    Конструктор / деструктор
*/

DepthStencilState::DepthStencilState (const DeviceManager& manager, const DepthStencilDesc& in_desc)
  : DeviceObject (manager)
  , desc (in_desc)
{
  try
  {
      //преобразование дескриптора

    D3D11_DEPTH_STENCIL_DESC dx_desc;

    memset (&dx_desc, 0, sizeof (dx_desc));

    dx_desc.DepthEnable      = desc.depth_test_enable;
    dx_desc.DepthWriteMask   = desc.depth_write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dx_desc.DepthFunc        = convert (desc.depth_compare_mode, "desc.depth_compare_mode");
    dx_desc.StencilEnable    = desc.stencil_test_enable;
    dx_desc.StencilReadMask  = desc.stencil_read_mask;
    dx_desc.StencilWriteMask = desc.stencil_write_mask;

    convert (desc.stencil_desc [FaceMode_Front], dx_desc.FrontFace, "desc.stencil_desc[FaceMode_Front]");
    convert (desc.stencil_desc [FaceMode_Back], dx_desc.BackFace, "desc.stencil_desc[FaceMode_Back]");

      //создание состояния

    ID3D11DepthStencilState* dx_state = 0;

    check_errors ("ID3D11Device::CreateDepthStencilState", manager.GetDevice ().CreateDepthStencilState (&dx_desc, &dx_state));

    if (!dx_state)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateDepthStencilState failed");

    state = DxDepthStencilStatePtr (dx_state, false);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::DepthStencilState::DepthStencilState");
    throw;
  }
}

DepthStencilState::~DepthStencilState ()
{
}

/*
    Установка/изменение дескриптора
*/

void DepthStencilState::GetDesc (DepthStencilDesc& out_desc)
{
  out_desc = desc;
}
