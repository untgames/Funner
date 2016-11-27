#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

namespace
{

D3D11_BLEND convert (BlendArgument arg, const char* name)
{
  switch (arg)
  {
    case BlendArgument_Zero:                    return D3D11_BLEND_ZERO;
    case BlendArgument_One:                     return D3D11_BLEND_ONE;
    case BlendArgument_SourceAlpha:             return D3D11_BLEND_SRC_ALPHA;
    case BlendArgument_InverseSourceAlpha:      return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendArgument_SourceColor:             return D3D11_BLEND_SRC_COLOR;
    case BlendArgument_InverseSourceColor:      return D3D11_BLEND_INV_SRC_COLOR;
    case BlendArgument_DestinationColor:        return D3D11_BLEND_DEST_COLOR;
    case BlendArgument_DestinationAlpha:        return D3D11_BLEND_DEST_ALPHA;
    case BlendArgument_InverseDestinationColor: return D3D11_BLEND_INV_DEST_COLOR;
    case BlendArgument_InverseDestinationAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
    default:                                    throw xtl::make_argument_exception ("", name, arg);
  }
}

D3D11_BLEND_OP convert (BlendOperation op, const char* name)
{
  switch (op)
  {
    case BlendOperation_Add:                return D3D11_BLEND_OP_ADD;
    case BlendOperation_Subtraction:        return D3D11_BLEND_OP_SUBTRACT;
    case BlendOperation_ReverseSubtraction: return D3D11_BLEND_OP_REV_SUBTRACT;
    case BlendOperation_Min:                return D3D11_BLEND_OP_MIN;
    case BlendOperation_Max:                return D3D11_BLEND_OP_MAX;
    default:                                throw xtl::make_argument_exception ("", name, op);
  }
}

}

BlendState::BlendState (const DeviceManager& manager, const BlendDesc& in_desc)
  : DeviceObject (manager)
  , desc (in_desc)
{
  try
  {
      //преобразование дескриптора

    D3D11_BLEND_DESC dx_desc;

    memset (&dx_desc, 0, sizeof (dx_desc));

    dx_desc.AlphaToCoverageEnable  = desc.sample_alpha_to_coverage;
    dx_desc.IndependentBlendEnable = desc.independent_blend_enable;

    static const size_t RT_COUNT = DEVICE_RENDER_TARGET_SLOTS_COUNT < 8 ? DEVICE_RENDER_TARGET_SLOTS_COUNT : 8;

    for (size_t i=0; i<RT_COUNT; i++)
    {
      const RenderTargetBlendDesc&    src = desc.render_target [i];
      D3D11_RENDER_TARGET_BLEND_DESC& dst = dx_desc.RenderTarget [i];

      dst.BlendEnable    = src.blend_enable;
      dst.BlendOp        = convert (src.blend_color_operation, "src.blend_color_operation");
      dst.BlendOpAlpha   = convert (src.blend_alpha_operation, "src.blend_alpha_operation");
      dst.SrcBlend       = convert (src.blend_color_source_argument, "src.blend_color_source_argument");
      dst.SrcBlendAlpha  = convert (src.blend_alpha_source_argument, "src.blend_alpha_source_argument");
      dst.DestBlend      = convert (src.blend_color_destination_argument, "src.blend_color_destination_argument");
      dst.DestBlendAlpha = convert (src.blend_alpha_destination_argument, "src.blend_alpha_destination_argument");

      if (src.color_write_mask & ColorWriteFlag_Red)   dst.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
      if (src.color_write_mask & ColorWriteFlag_Green) dst.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
      if (src.color_write_mask & ColorWriteFlag_Blue)  dst.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
      if (src.color_write_mask & ColorWriteFlag_Alpha) dst.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    }

      //создание состояния

    ID3D11BlendState* dx_state = 0;

    check_errors ("ID3D11Device::CreateBlendState", manager.GetDevice ().CreateBlendState (&dx_desc, &dx_state));

    if (!dx_state)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateBlendState failed");

    state = DxBlendStatePtr (dx_state, false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::BlendState::BlendState");
    throw;
  }
}

BlendState::~BlendState ()
{
}

/*
    Получение дескриптора
*/

void BlendState::GetDesc (BlendDesc& out_desc)
{
  out_desc = desc;
}
