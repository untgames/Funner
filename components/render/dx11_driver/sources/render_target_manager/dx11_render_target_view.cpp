#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

View::View (const DeviceManager& device_manager, ITexture* in_texture, const ViewDesc& in_desc)
  : DeviceObject (device_manager)
  , view_type ()
  , desc (in_desc)
  , texture (in_texture)
{
  try
  {
      //проверка корректности текстуры и совместимости с текущим контекстом

    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");

    Texture* texture_impl = cast_object<Texture> (*this, in_texture, "", "texture");

       //проверка корректности дескриптора

    TextureDesc texture_desc;

    texture->GetDesc (texture_desc);

    if (in_desc.layer >= texture_desc.layers)
      throw xtl::make_range_exception ("", "desc.layer", in_desc.layer, texture_desc.layers);

      //установка дескриптора

    desc = in_desc;

      //создание отображения

    if (is_depth_stencil (texture_desc.format))
    {
      view_type = ViewType_DepthStencil;

      ID3D11DepthStencilView* dx_view = 0;

      D3D11_DEPTH_STENCIL_VIEW_DESC dsd;

      memset (&dsd, 0, sizeof (dsd));

      dsd.Flags = 0;

      switch (texture_desc.dimension)
      {
        case TextureDimension_1D:
        {
          D3D11_TEXTURE1D_DESC desc;

          static_cast<ID3D11Texture1D*> (&texture_impl->GetHandle ())->GetDesc (&desc);
          
          dsd.Format             = desc.Format;
          dsd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE1D;
          dsd.Texture1D.MipSlice = in_desc.mip_level;

          break;
        }
        case TextureDimension_2D:
        {
          D3D11_TEXTURE2D_DESC desc;

          static_cast<ID3D11Texture2D*> (&texture_impl->GetHandle ())->GetDesc (&desc);
          
          dsd.Format             = desc.Format;
          dsd.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
          dsd.Texture2D.MipSlice = in_desc.mip_level;

          break;
        }
        case TextureDimension_3D:
          throw xtl::format_operation_exception ("", "3D textures can't be used as depth-stencil target");
        case TextureDimension_Cubemap:
        {
          D3D11_TEXTURE2D_DESC desc;

          static_cast<ID3D11Texture2D*> (&texture_impl->GetHandle ())->GetDesc (&desc);
          
          dsd.Format                         = desc.Format;
          dsd.ViewDimension                  = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
          dsd.Texture2DArray.MipSlice        = in_desc.mip_level;
          dsd.Texture2DArray.FirstArraySlice = in_desc.layer;
          dsd.Texture2DArray.ArraySize       = 1;

          break;
        }      
        default:
          throw xtl::format_operation_exception ("", "Unknown texture format '%s'", get_name (texture_desc.dimension));
      }

      check_errors ("ID3D11Device::CreateDepthStencilView", device_manager.GetDevice ().CreateDepthStencilView (&texture_impl->GetHandle (), &dsd, &dx_view));

      if (!dx_view)
        throw xtl::format_operation_exception ("", "ID3D11Device::CreateDepthStencilView failed");

      view = DxViewPtr (dx_view, false);
    }
    else
    {
      view_type = ViewType_RenderTarget;

      D3D11_RENDER_TARGET_VIEW_DESC rtd;

      memset (&rtd, 0, sizeof (rtd));

      switch (texture_desc.dimension)
      {
        case TextureDimension_1D:
        {
          D3D11_TEXTURE1D_DESC desc;

          static_cast<ID3D11Texture1D*> (&texture_impl->GetHandle ())->GetDesc (&desc);
          
          rtd.Format             = desc.Format;
          rtd.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE1D;
          rtd.Texture1D.MipSlice = in_desc.mip_level;

          break;
        }
        case TextureDimension_2D:
        {
          D3D11_TEXTURE2D_DESC desc;

          static_cast<ID3D11Texture2D*> (&texture_impl->GetHandle ())->GetDesc (&desc);

          rtd.Format             = desc.Format;
          rtd.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
          rtd.Texture2D.MipSlice = in_desc.mip_level;

          break;
        }
        case TextureDimension_3D:
        {
          D3D11_TEXTURE3D_DESC desc;

          static_cast<ID3D11Texture3D*> (&texture_impl->GetHandle ())->GetDesc (&desc);
          
          rtd.Format                = desc.Format;
          rtd.ViewDimension         = D3D11_RTV_DIMENSION_TEXTURE3D;
          rtd.Texture3D.MipSlice    = in_desc.mip_level;
          rtd.Texture3D.FirstWSlice = in_desc.layer;
          rtd.Texture3D.WSize       = 1;

          break;
        }
        case TextureDimension_Cubemap:
        {
          D3D11_TEXTURE2D_DESC desc;

          static_cast<ID3D11Texture2D*> (&texture_impl->GetHandle ())->GetDesc (&desc);
          
          rtd.Format                         = desc.Format;
          rtd.ViewDimension                  = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
          rtd.Texture2DArray.MipSlice        = in_desc.mip_level;
          rtd.Texture2DArray.FirstArraySlice = in_desc.layer;
          rtd.Texture2DArray.ArraySize       = 1;

          break;
        }      
        default:
          throw xtl::format_operation_exception ("", "Unknown texture format '%s'", get_name (texture_desc.dimension));
      }

      ID3D11RenderTargetView* dx_view = 0;
    
      check_errors ("ID3D11Device::CreateRenderTargetView", device_manager.GetDevice ().CreateRenderTargetView (&texture_impl->GetHandle (), &rtd, &dx_view));

      if (!dx_view)
        throw xtl::format_operation_exception ("", "ID3D11Device::CreateRenderTargetView failed");

      view = DxViewPtr (dx_view, false);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::View::View");
    throw;
  }
}

View::~View ()
{
}

/*
    Получение целевой текстуры
*/

ITexture* View::GetTexture ()
{
  return texture.get ();
}

/*
    Получение дескриптора
*/

void View::GetDesc (ViewDesc& out_desc)
{
  out_desc = desc;
}
