#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace render::mid_level::debug::renderer2d;

/*
    Конструктор / деструктор
*/

Texture::Texture (size_t width, size_t height, PixelFormat in_format)
  : RenderTarget (width, height, RenderTargetType_Color)
  , format (in_format)
  , is_compressed (false)
{
  switch (format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::renderer2d::Texture::Texture", "format", format);
  }
  
  log.Printf ("Create texture %ux%u format=%s (id=%u)", width, height, get_name (format), Id ());
}

Texture::Texture (size_t width, size_t height, const char* in_format)
  : RenderTarget (width, height, RenderTargetType_Color)
  , format (PixelFormat_RGBA8)
  , is_compressed (true)
{
  if (!in_format)
    throw xtl::make_null_argument_exception ("render::mid_level::debug::renderer2d::Texture::Texture", "format");

  log.Printf ("Create texture %ux%u format=%s (id=%u)", width, height, in_format, Id ());
}

Texture::~Texture ()
{
  try
  {
    log.Printf ("Destroy texture2d (id=%u)", Id ());
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Копирование образа текстуры в картинку
*/

void Texture::CaptureImage (media::Image& image)
{
  if (is_compressed)
    throw xtl::format_operation_exception ("render::mid_level::debug::renderer2d::Texture::CaptureImage", "Can't capture compressed image (not supported)");

  media::Image (GetWidth (), GetHeight (), 1, get_image_format (format)).Swap (image);

  log.Printf ("Capture image from texture (id=%u)", Id ());  
}
