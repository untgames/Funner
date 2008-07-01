#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;
using namespace media;

/*
    Конструктор / деструктор
*/

Texture2d::Texture2d (size_t width, size_t height, media::PixelFormat in_format)
  : RenderTarget (width, height, RenderTargetType_Color), format (in_format)
{
  switch (format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGB16:
    case PixelFormat_BGR8:
    case PixelFormat_RGBA8:
    case PixelFormat_RGBA16:
    case PixelFormat_BGRA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::Texture2d::Texture2d", "format", format);
  }
  
  log.Printf ("Create texture %ux%u (format=%d)", width, height, format);
}

Texture2d::~Texture2d ()
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

void Texture2d::CaptureImage (media::Image& image)
{
  media::Image (GetWidth (), GetHeight (), 1, GetFormat ()).Swap (image);

  log.Printf ("Capture image from texture #%u", Id ());  
}