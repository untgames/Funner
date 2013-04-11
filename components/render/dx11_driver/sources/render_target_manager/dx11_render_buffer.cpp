#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

RenderBuffer::RenderBuffer (const DeviceManager& manager, const TextureDesc& desc, const DxResourcePtr& in_resource)
  : DeviceObject (manager)
  , view_type ()
  , resource (in_resource)
{
  static const char* METHOD_NAME = "render::low_level::dx11::RenderBuffer::RenderBuffer";  

    //проверка корректности дескриптора

  if (!resource)
    throw xtl::make_null_argument_exception (METHOD_NAME, "resource");

  switch (desc.format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
      view_type = ViewType_RenderTarget;
      break;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_D32:
    case PixelFormat_S8:
      view_type = ViewType_DepthStencil;
      break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:    
    case PixelFormat_ATC_RGB_AMD:
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported desc.format=%s", get_name (desc.format));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", desc.format);
  }

  static size_t BAD_BIND_FLAGS = ~(BindFlag_RenderTarget | BindFlag_DepthStencil);

  if (desc.bind_flags & BAD_BIND_FLAGS)
    throw xtl::make_argument_exception (METHOD_NAME, "Unsupported bindable flags desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));

  switch (desc.access_flags)
  {
    case 0:
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_ReadWrite:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.access_flags", desc.access_flags);
  }

  switch (desc.usage_mode)
  {
    case UsageMode_Default:
    case UsageMode_Static:
    case UsageMode_Dynamic:
    case UsageMode_Stream:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.usage_mode", desc.usage_mode);
      break;
  }

  switch (desc.dimension)
  {
    case TextureDimension_2D:
      break;
    case TextureDimension_1D:
    case TextureDimension_3D:
    case TextureDimension_Cubemap:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported render-buffer dimension desc.dimension=%s", get_name (desc.dimension));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.dimension", desc.dimension);
  }

  if (desc.generate_mips_enable)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Automatic mip-map generation for render-buffers not supported");
}

/*
    Получение дескриптора
*/

void RenderBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
}

/*
    Работа с данными
*/

void RenderBuffer::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void RenderBuffer::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
