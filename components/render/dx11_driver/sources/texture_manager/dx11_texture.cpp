#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

namespace
{

///Дескриптор mip-уровня текстуры
struct MipLevelDesc
{
  size_t width, height; //ширина / высота изображения на уровне
};

}

/*
    Описание реализации
*/

struct Texture::Impl
{
  TextureDesc             desc;       //дескриптор текстуры
  DxResourcePtr           texture;    //ресурс
  DxShaderResourceViewPtr view;       //отображение
  size_t                  mips_count; //количество мип-уровней

/// Конструктор
  Impl (const TextureDesc& in_desc, const DxResourcePtr& in_texture = DxResourcePtr ())
    : desc (in_desc)
    , texture (in_texture)
    , mips_count ()
  {
  }

/// Получение дескриптора уровня текстуры
  void GetMipLevelDesc (size_t mip_level, MipLevelDesc& out_desc) const
  {
    size_t level_width  = desc.width >> mip_level,
           level_height = desc.height >> mip_level;
    
    if (!level_width)  level_width  = 1;
    if (!level_height) level_height = 1;

    out_desc.width  = level_width;
    out_desc.height = level_height;
  }
};

/*
    Конструктор
*/

Texture::Texture (const DeviceManager& manager, const TextureDesc& in_desc, const DxResourcePtr& in_texture)
  : DeviceObject (manager)
  , impl (new Impl (in_desc, in_texture))
{
  static const char* METHOD_NAME = "render::low_level::dx11::Texture::Texture(const DeviceManager&,const TextureDesc&,const DxResourcePtr&)";

    //проверка корректности дескриптора

  if (!impl->texture)
    throw xtl::make_null_argument_exception (METHOD_NAME, "resource");

  switch (impl->desc.access_flags)
  {
    case 0:
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_ReadWrite:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.access_flags", impl->desc.access_flags);
  }

  switch (impl->desc.usage_mode)
  {
    case UsageMode_Default:
    case UsageMode_Static:
    case UsageMode_Dynamic:
    case UsageMode_Stream:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.usage_mode", impl->desc.usage_mode);
  }

  switch (impl->desc.dimension)
  {
    case TextureDimension_2D:
      break;
    case TextureDimension_1D:
    case TextureDimension_3D:
    case TextureDimension_Cubemap:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render-buffer dimension desc.dimension=%s", get_name (impl->desc.dimension));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.dimension", impl->desc.dimension);
  }

  if (impl->desc.generate_mips_enable)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Automatic mip-map generation for render-buffers not supported");
}

namespace
{

DXGI_FORMAT get_dx11_format (PixelFormat fmt)
{
  switch (fmt)
  {
    case PixelFormat_RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat_L8:    return DXGI_FORMAT_R8_UINT;
    case PixelFormat_A8:    return DXGI_FORMAT_A8_UNORM;
    case PixelFormat_LA8:   return DXGI_FORMAT_R8G8_UINT;
    case PixelFormat_D16:   return DXGI_FORMAT_D16_UNORM;
    case PixelFormat_D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
      break;
    case PixelFormat_D32:
    case PixelFormat_D24X8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    case PixelFormat_S8:
    case PixelFormat_RGB8:
    case PixelFormat_RGB_PVRTC2:  
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
    case PixelFormat_ATC_RGB_AMD:
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      throw xtl::format_operation_exception ("render::low_level::dx11::get_dx11_format", "Format %s can't be found in DX11", get_name (fmt));
    default:
      throw xtl::make_argument_exception ("render::low_level::dx11::get_dx11_format", "format", fmt);
  }
}

template <class DstDesc>
void fill_desc (const TextureDesc& src_desc, DstDesc& dst_desc)
{
  memset (&dst_desc, 0, sizeof (dst_desc));

  dst_desc.Width     = src_desc.width;
  dst_desc.MipLevels = 0;
  dst_desc.Format    = get_dx11_format (src_desc.format);

  switch (src_desc.usage_mode)
  {
    case UsageMode_Default:
      dst_desc.Usage = D3D11_USAGE_DEFAULT;
      break;
    case UsageMode_Static:
      dst_desc.Usage = D3D11_USAGE_IMMUTABLE;
      break;
    case UsageMode_Dynamic:
      dst_desc.Usage = D3D11_USAGE_DYNAMIC;
      break;
    case UsageMode_Stream:
      dst_desc.Usage = D3D11_USAGE_STAGING;
      break;
    default:
      throw xtl::make_argument_exception ("", "desc.usage_mode", src_desc.usage_mode);
  }

  struct FlagMap
  {
    BindFlag src_flag;
    UINT     dst_flag;
  };

  static const FlagMap flags [] = {
    {BindFlag_VertexBuffer,   D3D11_BIND_VERTEX_BUFFER},
    {BindFlag_IndexBuffer,    D3D11_BIND_INDEX_BUFFER},
    {BindFlag_ConstantBuffer, D3D11_BIND_CONSTANT_BUFFER},
    {BindFlag_Texture,        D3D11_BIND_SHADER_RESOURCE},
    {BindFlag_RenderTarget,   D3D11_BIND_RENDER_TARGET},
    {BindFlag_DepthStencil,   D3D11_BIND_DEPTH_STENCIL},
  };

  static size_t flags_count = sizeof (flags) / sizeof (*flags);

  for (size_t i=0; i<flags_count; i++)
    if (src_desc.bind_flags & flags [i].src_flag)
      dst_desc.BindFlags |= flags [i].dst_flag;    

  if (src_desc.access_flags & AccessFlag_Read)  dst_desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
  if (src_desc.access_flags & AccessFlag_Write) dst_desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;

  if (src_desc.generate_mips_enable) dst_desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
}

}

Texture::Texture (const DeviceManager& device_manager, const TextureDesc& desc, const TextureData* data)
  : DeviceObject (device_manager)
  , impl (new Impl (desc))
{
  try
  {
    if (data)
      throw xtl::make_null_argument_exception ("", "data");

    switch (desc.dimension)
    {
      case TextureDimension_1D:
      {
        D3D11_TEXTURE1D_DESC dst_desc;
 
        fill_desc (desc, dst_desc);

        ID3D11Texture1D* texture = 0;

        check_errors ("D3D11Device::CreateTexture1D", GetDeviceManager ().GetDevice ().CreateTexture1D (&dst_desc, 0, &texture));

        if (!texture)
          throw xtl::format_operation_exception ("", "D3D11Device::CreateTexture1D failed");

        impl->texture    = DxResourcePtr (texture, false);
        impl->mips_count = get_mips_count (desc.width);

        break;
      }
      case TextureDimension_2D:
      {
        D3D11_TEXTURE2D_DESC dst_desc;
 
        fill_desc (desc, dst_desc);

        dst_desc.Height             = desc.height;
        dst_desc.ArraySize          = desc.layers;
        dst_desc.SampleDesc.Count   = 1;
        dst_desc.SampleDesc.Quality = 0;

        ID3D11Texture2D* texture = 0;

        check_errors ("D3D11Device::CreateTexture2D", GetDeviceManager ().GetDevice ().CreateTexture2D (&dst_desc, 0, &texture));

        if (!texture)
          throw xtl::format_operation_exception ("", "D3D11Device::CreateTexture2D failed");

        impl->texture    = DxResourcePtr (texture, false);
        impl->mips_count = get_mips_count (desc.width, desc.height);

        break;
      }
      case TextureDimension_3D:
      {
        D3D11_TEXTURE3D_DESC dst_desc;
 
        fill_desc (desc, dst_desc);

        dst_desc.Height = desc.height;
        dst_desc.Depth  = desc.layers;

        ID3D11Texture3D* texture = 0;

        check_errors ("D3D11Device::CreateTexture3D", GetDeviceManager ().GetDevice ().CreateTexture3D (&dst_desc, 0, &texture));

        if (!texture)
          throw xtl::format_operation_exception ("", "D3D11Device::CreateTexture3D failed");

        impl->texture    = DxResourcePtr (texture, false);
        impl->mips_count = get_mips_count (desc.width, desc.height, desc.layers);

        break;
      }
      case TextureDimension_Cubemap:
      {
        D3D11_TEXTURE2D_DESC dst_desc;
 
        fill_desc (desc, dst_desc);

        if (desc.layers != 6)
          throw xtl::format_operation_exception ("", "Cubemap must have 6 layers");

        dst_desc.Height     = desc.height;
        dst_desc.ArraySize  = desc.layers;
        dst_desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

        ID3D11Texture2D* texture = 0;

        check_errors ("D3D11Device::CreateTexture2D", GetDeviceManager ().GetDevice ().CreateTexture2D (&dst_desc, 0, &texture));

        if (!texture)
          throw xtl::format_operation_exception ("", "D3D11Device::CreateTexture2D failed");

        impl->texture    = DxResourcePtr (texture, false);
        impl->mips_count = get_mips_count (desc.width, desc.height);

        break;
      }
      default:
        throw xtl::make_argument_exception ("", "TextureDesc::dimension", desc.dimension);
    }

      //принудительное создание отображения

//    GetShaderResourceView ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Texture::Texture(const DeviceManager&,const TextureDesc&)");
    throw;
  }
}

Texture::~Texture ()
{
}

/*
    Получение дескриптора
*/

void Texture::GetDesc (TextureDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Получение низкоуровневого дескриптора
*/

ID3D11Resource& Texture::GetHandle ()
{
  return *impl->texture;
}

ID3D11ShaderResourceView& Texture::GetShaderResourceView ()
{
  try
  {
    if (impl->view)
      return *impl->view;

      //получение размерности ресурса

    D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;

    impl->texture->GetType (&dimension);

      //создание отображения

    D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;

    memset (&view_desc, 0, sizeof (view_desc));

    switch (dimension)
    {
      case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
      {
        D3D11_TEXTURE1D_DESC tex_desc;

        static_cast<ID3D11Texture1D*> (impl->texture.get ())->GetDesc (&tex_desc);

        view_desc.Format                    = tex_desc.Format;
        view_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE1D;
        view_desc.Texture1D.MostDetailedMip = 0;
        view_desc.Texture1D.MipLevels       = -1;

        break;
      }
      case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
      {
        D3D11_TEXTURE2D_DESC tex_desc;

        static_cast<ID3D11Texture2D*> (impl->texture.get ())->GetDesc (&tex_desc);

        view_desc.Format                    = tex_desc.Format;
        view_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        view_desc.Texture2D.MostDetailedMip = 0;
        view_desc.Texture2D.MipLevels       = -1;

        break;
      }
      case D3D11_RESOURCE_DIMENSION_TEXTURE3D:        
      {
        D3D11_TEXTURE3D_DESC tex_desc;

        static_cast<ID3D11Texture3D*> (impl->texture.get ())->GetDesc (&tex_desc);

        view_desc.Format                    = tex_desc.Format;
        view_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE3D;
        view_desc.Texture3D.MostDetailedMip = 0;
        view_desc.Texture3D.MipLevels       = -1;

        break;
      }
      case D3D11_RESOURCE_DIMENSION_BUFFER:
        throw xtl::format_operation_exception ("", "Can't create shader resource view from buffer");
      default:
        throw xtl::format_operation_exception ("", "Unknown resource dimension %u", dimension);
    }

    ID3D11ShaderResourceView* dx_view = 0;

    check_errors ("ID3D11Device::CreateShaderResourceView", GetDeviceManager ().GetDevice ().CreateShaderResourceView (impl->texture.get (), &view_desc, &dx_view));

    if (dx_view)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateShaderResourceView failed");

    impl->view = DxShaderResourceViewPtr (dx_view, false);

    return *impl->view; 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Texture::GetShaderResourceView");
    throw;
  }
}

/*
    Работа с данными
*/

void Texture::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  try
  {
      //проверка возможности записи
    
    if (!(impl->desc.access_flags & AccessFlag_Write))
      throw xtl::format_operation_exception ("", "Can't set texture data (no AccessFlag_Write in desc.access_flags)");

      //проверка корректности номеров слоя и mip-уровня

    if (layer >= impl->desc.layers)
      throw xtl::make_range_exception ("", "layer", layer, impl->desc.layers);

    if (mip_level >= impl->mips_count)
      throw xtl::make_range_exception ("", "mip_level", mip_level, impl->mips_count);

      //сохранение неотсеченных размеров

    size_t unclamped_width  = width,
           unclamped_height = height;

      //получение дескриптора mip-уровня и информации о слое текстуры

    MipLevelDesc level_desc;

    impl->GetMipLevelDesc (mip_level, level_desc);

      //отсечение

    if (x >= level_desc.width || y >= level_desc.height)
      return;

    if (x + width > level_desc.width)
      width = level_desc.width - x;

    if (y + height > level_desc.height)
      height = level_desc.height - y;    

    if (!width || !height)
      return;

    if (!buffer)
      throw xtl::make_null_argument_exception ("", "buffer");    

      //проверка совместимости форматов

    bool is_depth_stencil_source_format = is_depth_stencil (source_format),
         is_depth_stencil_tex_format    = is_depth_stencil (impl->desc.format),
         is_compatible                  = (is_depth_stencil_tex_format  && is_depth_stencil_source_format) ||
                                          (!is_depth_stencil_tex_format && !is_depth_stencil_source_format);

    if (!is_compatible)
      throw xtl::format_not_supported_exception ("", "Texture format %s incompatible with source_format %s", get_name (impl->desc.format), get_name (source_format));

    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Texture::SetData");
    throw;
  }
}

void Texture::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
