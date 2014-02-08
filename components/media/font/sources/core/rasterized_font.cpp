#include "shared.h"

using namespace media;

/*
   Реализация растеризованного шрифта
*/

namespace media
{

struct RasterizedFont::Impl : public xtl::reference_counter
{
  xtl::com_ptr<IFontRasterizer> rasterizer;

  Impl (IFontRasterizer* in_rasterizer)
    : rasterizer (in_rasterizer)
    {}
};

}

/*
   Конструктор/деструктор
*/

RasterizedFont::RasterizedFont (IFontRasterizer* rasterizer)
  : impl (new Impl (rasterizer))
{
}

RasterizedFont::RasterizedFont (const RasterizedFont& source)
  : impl (source.impl)
{
  addref (impl);
}

RasterizedFont::~RasterizedFont ()
{
  release (impl);
}

/*
   Присваивание
*/

RasterizedFont& RasterizedFont::operator = (const RasterizedFont& source)
{
  RasterizedFont (source).Swap (*this);

  return *this;
}

/*
   Растеризованные глифы
*/

const RasterizedGlyphInfo* RasterizedFont::RasterizedGlyphs () const
{
  return impl->rasterizer->RasterizedGlyphs ();
}

/*
   Количество картинок
*/

size_t RasterizedFont::ImagesCount () const
{
  return impl->rasterizer->ImagesCount ();
}

/*
   Построение картинки
*/

void RasterizedFont::BuildImage (size_t image_index, media::Image& out_image) const
{
  impl->rasterizer->BuildImage (image_index, out_image);
}

/*
   Обмен
*/

void RasterizedFont::Swap (RasterizedFont& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (RasterizedFont& font1, RasterizedFont& font2)
{
  font1.Swap (font2);
}

}
