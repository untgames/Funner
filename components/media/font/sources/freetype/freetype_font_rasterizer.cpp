#include "shared.h"

using namespace media;
using namespace media::freetype;

namespace
{

static const char* LOG_NAME = "media.freetype.FreetypeFontRasterizer";

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
  media::PixelFormat     image_format;    //формат атласа
  RasterizedGlyphsBuffer glyphs;          //информация о символах
  media::AtlasBuilder    atlas_builder;   //построитель атласа

  ///Конструктор
  Impl (const RasterizedFontCreationParams& params, const FreetypeRasterizedFont& in_rasterized_font)
    : rasterized_font (in_rasterized_font)
    , image_format (params.image_format)
    , glyphs (in_rasterized_font.GlyphsCount ())
  {
    atlas_builder.SetMargin       (params.glyph_margin);
    atlas_builder.SetMaxImageSize (params.max_image_size);

    unsigned int pack_flags = AtlasPackFlag_Fast;

    if (params.pot)
      pack_flags |= AtlasPackFlag_PowerOfTwoEdges;

    atlas_builder.SetPackFlags (pack_flags);

    unsigned int unique_glyphs_count = rasterized_font.UniqueGlyphsCount ();

    const math::vec2ui*   current_glyph_size   = rasterized_font.GlyphsSizes ();
    const unsigned char** current_glyph_bitmap = rasterized_font.GlyphsBitmaps ();

    for (unsigned int i = 0; i < unique_glyphs_count; i++, current_glyph_size++, current_glyph_bitmap++)
      atlas_builder.Insert (current_glyph_size->x, current_glyph_size->y, media::PixelFormat_L8, *current_glyph_bitmap, false, "", i);

    RasterizedGlyphsBuffer unique_glyphs (unique_glyphs_count);

    for (unsigned int i = 0, count = atlas_builder.AtlasesCount (); i < count; i++)
    {
      Atlas atlas;

      atlas_builder.BuildAtlas (i, "", atlas);

      for (unsigned int j = 0, tiles_count = atlas.TilesCount (); j < tiles_count; j++)
      {
        const media::Tile&   tile  = atlas.Tile (j);
        RasterizedGlyphInfo& glyph = unique_glyphs.data () [tile.tag];

        glyph.image_index = i;
        glyph.width       = tile.size.x;
        glyph.height      = tile.size.y;
        glyph.x_pos       = tile.origin.x;
        glyph.y_pos       = tile.origin.y;
      }
    }

    const unsigned int*  current_mapping = rasterized_font.GlyphsMap ();
    RasterizedGlyphInfo* current_glyph   = glyphs.data ();

    for (unsigned int i = 0, count = rasterized_font.GlyphsCount (); i < count; i++, current_mapping++, current_glyph++)
      memcpy (current_glyph, unique_glyphs.data () + *current_mapping, sizeof (RasterizedGlyphInfo));
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

unsigned int FreetypeFontRasterizer::ImagesCount ()
{
  return impl->atlas_builder.AtlasesCount ();
}

void FreetypeFontRasterizer::BuildImage (unsigned int image_index, media::Image& out_image)
{
  try
  {
    impl->atlas_builder.BuildAtlasImage (image_index, out_image);

    if (out_image.Format () != impl->image_format && impl->image_format != media::PixelFormat_Default)
    {
      common::Log (LOG_NAME).Printf ("Requested pixel format '%s' requires image convertation from '%s'",
    		                         get_format_name (impl->image_format), get_format_name (out_image.Format ()));
      out_image.Convert (impl->image_format);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::freetype::FreetypeFontRasterizer::BuildImage");
    throw;
  }
}
