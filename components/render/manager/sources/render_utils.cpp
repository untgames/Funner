#include "shared.h"

using namespace render::manager;

/*
    Получение имени формата
*/

namespace render
{

namespace manager
{

const char* get_name (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:        return "rgb8";
    case PixelFormat_RGBA8:       return "rgba8";
    case PixelFormat_L8:          return "l8";
    case PixelFormat_A8:          return "a8";
    case PixelFormat_LA8:         return "la8";
    case PixelFormat_DXT1:        return "dxt1";
    case PixelFormat_DXT3:        return "dxt3";
    case PixelFormat_DXT5:        return "dxt5";
    case PixelFormat_RGB_PVRTC2:  return "rgb_pvrtc2";
    case PixelFormat_RGB_PVRTC4:  return "rgb_pvrtc4";
    case PixelFormat_RGBA_PVRTC2: return "rgba_pvrtc2";
    case PixelFormat_RGBA_PVRTC4: return "rgba_pvrtc4";
    case PixelFormat_ATC_RGB_AMD:                     return "atc";
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:     return "atci";
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD: return "atca";
    case PixelFormat_D16:         return "d16";
    case PixelFormat_D24X8:       return "d24x8";
    case PixelFormat_D24S8:       return "d24s8";
    case PixelFormat_D32:         return "d32";
    case PixelFormat_S8:          return "s8";    
    default:
      throw xtl::make_argument_exception ("render::manager::get_name(render::manager::PixelFormat)", "format", format);
  }
}

}

}
