#include "shared.h"

using namespace media;
using namespace media::xfont;

namespace
{

static const char* LOG_NAME = "media.xfont.XFontFontRasterizer";

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k)
{
  if (!k)
    return 1;

  if (!(k & (k-1)))
    return k;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

//проверка размера на степень двойки
bool is_power_of_two (size_t size)
{
  return ((size - 1) & size) == 0;
}

}

namespace media
{

namespace xfont
{

/*
   Растеризатор шрифта
*/

struct XFontFontRasterizer::Impl
{
  RasterizedFontCreationParams params;      //параметры растеризации
  RasterizedFontParamsPtr      font_params; //данные о шрифте

  ///Конструктор
  Impl (const RasterizedFontCreationParams& in_params, const RasterizedFontParamsPtr& in_font_params)
    : font_params (in_font_params)
  {
    memcpy (&params, &in_params, sizeof (params));
  }
};

}

}

/*
   Конструктор / деструктор
*/

XFontFontRasterizer::XFontFontRasterizer (const RasterizedFontCreationParams& params, const RasterizedFontParamsPtr& font_params)
  : impl (new Impl (params, font_params))
  {}

XFontFontRasterizer::~XFontFontRasterizer ()
{
  delete impl;
}

/*
   Получение информации о растрированных символах
*/

const RasterizedGlyphInfo* XFontFontRasterizer::RasterizedGlyphs ()
{
  return impl->font_params->rasterized_glyphs.data ();
}

/*
   Построение картинок
*/

size_t XFontFontRasterizer::ImagesCount ()
{
  return impl->font_params->images_count;
}

void XFontFontRasterizer::BuildImage (size_t image_index, media::Image& out_image)
{
  try
  {
    out_image.Load (common::format (impl->font_params->image_name_format.c_str (), image_index).c_str ());

    size_t image_width  = out_image.Width (),
           image_height = out_image.Height ();

    if (impl->params.pot)
    {
      image_width  = get_next_higher_power_of_two (image_width);
      image_height = get_next_higher_power_of_two (image_height);
    }

    size_t max_side = stl::max (image_width, image_height);

    if (max_side > impl->params.max_image_size)
      throw xtl::format_operation_exception ("", "Can't build image, size %lu is larger than max size %lu", max_side, impl->params.max_image_size);

    if (image_width != out_image.Width () || image_height != out_image.Height ())
    {
      common::Log (LOG_NAME).Printf ("Rasterizing params requires image enlarging to pot");

      Image pot_image (image_width, image_height, 1, out_image.Format ());

      pot_image.PutImage (0, 0, 0, out_image.Width (), out_image.Height (), 1, out_image.Format (), out_image.Bitmap ());

      out_image.Swap (pot_image);
    }

    if (out_image.Format () != impl->params.image_format && impl->params.image_format != media::PixelFormat_Default)
    {
      common::Log (LOG_NAME).Printf ("Requested pixel format '%s' requires image convertation from '%s'", impl->params.image_format, out_image.Format ());
      out_image.Convert (impl->params.image_format);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::xfont::XFontFontRasterizer::BuildImage");
    throw;
  }
}
