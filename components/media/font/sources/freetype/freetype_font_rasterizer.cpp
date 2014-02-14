#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

typedef xtl::uninitialized_storage<RasterizedGlyphInfo> RasterizedGlyphsBuffer;

}

namespace media
{

namespace freetype
{

/*
   Растеризатор шрифта
*/

struct FreetypeFontRasterizer::Impl
{
  FreetypeRasterizedFont rasterized_font; //растеризованный шрифт
  size_t                 images_count;    //количество картинок
  RasterizedGlyphsBuffer glyphs;          //информация о символах

  ///Конструктор
  Impl (const RasterizedFontCreationParams& params, const FreetypeRasterizedFont& in_rasterized_font)
    : rasterized_font (in_rasterized_font)
    , images_count (0)
    , glyphs (in_rasterized_font.GlyphsCount ())
  {
    rasterized_font.UniqueGlyphsCount ();
  }
};

}

}

/*
   Конструктор / деструктор
*/

FreetypeFontRasterizer::FreetypeFontRasterizer (const RasterizedFontCreationParams& params, const FreetypeRasterizedFont& rasterized_font)
  : impl (new Impl (params, rasterized_font))
  {}

FreetypeFontRasterizer::~FreetypeFontRasterizer ()
{
  delete impl;
}

/*
   Получение информации о растрированных символах
*/

const RasterizedGlyphInfo* FreetypeFontRasterizer::RasterizedGlyphs ()
{
  return impl->glyphs.data ();
}

/*
   Построение картинок
*/

size_t FreetypeFontRasterizer::ImagesCount ()
{
  return impl->images_count;
}

void FreetypeFontRasterizer::BuildImage (size_t image_index, media::Image& out_image)
{
  //TODO implement this
}
