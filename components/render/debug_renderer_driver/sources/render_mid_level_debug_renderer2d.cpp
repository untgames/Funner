#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

Renderer2d::Renderer2d ()
{
}

/*
    Создание ресурсов
*/

ITexture* Renderer2d::CreateTexture (const media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::CreateTexture(const media::Image&)");
}

ITexture* Renderer2d::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::CreateTexture(size_t,size_t,media::PixelFormat)");
}

ISpriteList* Renderer2d::CreateSpriteList (size_t sprites_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::CreateSpriteList");
}

/*
    Добавление кадра на отрисовку
*/

void Renderer2d::AddFrame (renderer2d::Frame*)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::AddFrame");
}

/*
    Конец отрисовки
*/

void Renderer2d::DrawFrames ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::debug::Renderer2d::DrawFrames");
}
