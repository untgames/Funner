#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

namespace
{

D3D11_TEXTURE_ADDRESS_MODE get_address_mode (TexcoordWrap wrap, const char* arg_name)
{
  switch (wrap)
  {
    case TexcoordWrap_Repeat:         return D3D11_TEXTURE_ADDRESS_WRAP;
    case TexcoordWrap_Mirror:         return D3D11_TEXTURE_ADDRESS_MIRROR;
    case TexcoordWrap_Clamp:          return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TexcoordWrap_ClampToBorder:  return D3D11_TEXTURE_ADDRESS_BORDER;
    default:                          throw xtl::make_argument_exception ("", arg_name, wrap);
  }
}

}

SamplerState::SamplerState (const DeviceManager& device_manager, const SamplerDesc& in_desc)
  : DeviceObject (device_manager)
  , sampler ()
  , desc (in_desc)
{
  try
  {
    D3D11_SAMPLER_DESC sampler_desc;

    memset (&sampler_desc, 0, sizeof (sampler_desc));

    switch (desc.min_filter)
    {
      case TexMinFilter_Point:
        switch (desc.mag_filter)
        {
          case TexMagFilter_Point:
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
          case TexMagFilter_Default:
          case TexMagFilter_Linear:
            sampler_desc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
          default:
            throw xtl::make_argument_exception ("", "desc.mag_filter", desc.mag_filter);
        }

        break;
      case TexMinFilter_Linear:
        switch (desc.mag_filter)
        {
          case TexMagFilter_Point:
            sampler_desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
          case TexMagFilter_Default:
          case TexMagFilter_Linear:
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
          default:
            throw xtl::make_argument_exception ("", "desc.mag_filter", desc.mag_filter);
        }

        break;
      case TexMinFilter_PointMipPoint:
        switch (desc.mag_filter)
        {
          case TexMagFilter_Point:
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
          case TexMagFilter_Default:
          case TexMagFilter_Linear:
            sampler_desc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
          default:
            throw xtl::make_argument_exception ("", "desc.mag_filter", desc.mag_filter);
        }

        break;
      case TexMinFilter_LinearMipPoint:
        switch (desc.mag_filter)
        {
          case TexMagFilter_Point:
            sampler_desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
          case TexMagFilter_Default:
          case TexMagFilter_Linear:
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
          default:
            throw xtl::make_argument_exception ("", "desc.mag_filter", desc.mag_filter);
        }

        break;
      case TexMinFilter_PointMipLinear:
        switch (desc.mag_filter)
        {
          case TexMagFilter_Point:
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
          case TexMagFilter_Default:
          case TexMagFilter_Linear:
            sampler_desc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
          default:
            throw xtl::make_argument_exception ("", "desc.mag_filter", desc.mag_filter);
        }

        break;
      case TexMinFilter_Default:
      case TexMinFilter_LinearMipLinear:
        switch (desc.mag_filter)
        {
          case TexMagFilter_Point:
            sampler_desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
          case TexMagFilter_Default:
          case TexMagFilter_Linear:
            sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
          default:
            throw xtl::make_argument_exception ("", "desc.mag_filter", desc.mag_filter);
        }

        break;
      default:
        throw xtl::make_argument_exception ("", "desc.min_filter", desc.min_filter);
    }

    switch (desc.comparision_function)
    {
      case CompareMode_AlwaysFail:   sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;         break;
      case CompareMode_AlwaysPass:   sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;        break;
      case CompareMode_Equal:        sampler_desc.ComparisonFunc = D3D11_COMPARISON_EQUAL;         break;
      case CompareMode_NotEqual:     sampler_desc.ComparisonFunc = D3D11_COMPARISON_NOT_EQUAL;     break;
      case CompareMode_Less:         sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS;          break;
      case CompareMode_LessEqual:    sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;    break;
      case CompareMode_Greater:      sampler_desc.ComparisonFunc = D3D11_COMPARISON_GREATER;       break;
      case CompareMode_GreaterEqual: sampler_desc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
      default:                       throw xtl::make_argument_exception ("", "desc.comparision_function", desc.comparision_function);  
    }

    if (desc.comparision_function != CompareMode_AlwaysPass)
      sampler_desc.Filter = (D3D11_FILTER)(sampler_desc.Filter + 0x80); //see D3D11_FILTER enumeration

    sampler_desc.AddressU      = get_address_mode (desc.wrap_u, "desc.wrap_u");
    sampler_desc.AddressV      = get_address_mode (desc.wrap_v, "desc.wrap_v");
    sampler_desc.AddressW      = get_address_mode (desc.wrap_w, "desc.wrap_w");
    sampler_desc.MipLODBias    = desc.mip_lod_bias;
    sampler_desc.MinLOD        = desc.min_lod;
    sampler_desc.MaxLOD        = desc.max_lod;
    sampler_desc.MaxAnisotropy = desc.max_anisotropy;
    
    memcpy (sampler_desc.BorderColor, desc.border_color, sizeof (desc.border_color));

    ID3D11SamplerState* dx_sampler = 0;

    check_errors ("ID3D11Device::CreateSamplerState", device_manager.GetDevice ().CreateSamplerState (&sampler_desc, &dx_sampler));

    if (!dx_sampler)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateSamplerState failed");

    sampler = DxSamplerStatePtr (dx_sampler, false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SamplerState::SamplerState");
    throw;
  }
}

SamplerState::~SamplerState ()
{
}

/*
    Изменение / получение дескриптора
*/

void SamplerState::SetDesc (const SamplerDesc&)
{
  throw xtl::format_operation_exception ("render::low_level::dx11::SamplerState::SetDesc", "SetDesc is not supported");
}

void SamplerState::GetDesc (SamplerDesc& out_desc)
{
  out_desc = desc;
}
