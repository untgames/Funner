#include "shared.h"

using namespace media;
using namespace media::xfont;

namespace
{

IFontRasterizer* font_rasterizer_creator (const RasterizedFontCreationParams& params, RasterizedFontParamsPtr font_params)
{
  return new XFontFontRasterizer (params, font_params);
}

}

namespace media
{

namespace xfont
{

/*
   Шрифт
*/

struct XFontFontDesc::Impl
{
  FontBuilder font; //шрифт

  ///Конструктор / деструктор
  Impl (const char* file_name)
  {
    try
    {
      common::Parser           p (file_name);
      common::ParseLog         log = p.Log ();
      common::Parser::Iterator iter = p.Root ().First("Font");

      for (size_t i = 0; i < log.MessagesCount (); i++)
        switch (log.MessageType (i))
        {
          case common::ParseLogMessageType_Error:
          case common::ParseLogMessageType_FatalError:
            throw xtl::format_operation_exception ("", log.Message (i));
          default:
            break;
        }

      if (!iter)
        throw xtl::format_operation_exception ("", "Incorrect file format, no 'Font' root tag");


      font.SetSource         (file_name);
      font.Rename            (common::get<const char*> (*iter, "Name", ""));
      font.SetFamilyName     (common::get<const char*> (*iter, "FamilyName", ""));
      font.SetStyleName      (common::get<const char*> (*iter, "StyleName", ""));
      font.SetFirstGlyphCode (common::get<size_t> (*iter, "FirstCharCode"));
      font.SetFontSize       (common::get<size_t> (*iter, "FontSize"));

      size_t glyph_count = 0;

      for (common::Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; ++i, glyph_count++);

      font.SetGlyphsCount (glyph_count);

      RasterizedFontParamsPtr rasterize_params (new RasterizedFontParams, false);

      rasterize_params->images_count      = common::get (*iter, "ImagesCount", 1);
      rasterize_params->image_name_format = common::get<const char*> (*iter, "FontFile");

      rasterize_params->rasterized_glyphs.resize (glyph_count);

      GlyphInfo*           glyph_info            = font.Glyphs ();
      RasterizedGlyphInfo* rasterized_glyph_info = rasterize_params->rasterized_glyphs.data ();

      for (common::Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; ++i, glyph_info++, rasterized_glyph_info++)
      {
        glyph_info->width     = common::get<float> (*i, "Width");
        glyph_info->height    = common::get<float> (*i, "Height");
        glyph_info->advance_x = common::get<float> (*i, "AdvanceX");
        glyph_info->advance_y = common::get<float> (*i, "AdvanceY");
        glyph_info->bearing_x = common::get<float> (*i, "BearingX");
        glyph_info->bearing_y = common::get<float> (*i, "BearingY");

        rasterized_glyph_info->width       = glyph_info->width;
        rasterized_glyph_info->height      = glyph_info->height;
        rasterized_glyph_info->image_index = common::get (*i, "ImageIndex", 0u);
        rasterized_glyph_info->x_pos       = common::get<size_t> (*i, "XPos");
        rasterized_glyph_info->y_pos       = common::get<size_t> (*i, "YPos");
      }

      for (common::Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; ++i)
      {
        KerningInfo kerning_info;

        kerning_info.x_kerning = common::get<float> (*i, "XKerning");
        kerning_info.y_kerning = common::get<float> (*i, "YKerning");

        font.InsertKerning (common::get<size_t> (*i, "LeftGlyph"), common::get<size_t> (*i, "RightGlyph"), kerning_info);
      }

      font.SetRasterizer (xtl::bind (&font_rasterizer_creator, _1, rasterize_params));
    }
    catch (xtl::exception& e)
    {
      e.touch ("media::xfont::XFontFontDesc::XFontFontDesc");
      throw;
    }
  }
};

}

}

/*
   Конструктор / деструктор
*/

XFontFontDesc::XFontFontDesc (const char* file_name)
  : impl (new Impl (file_name))
  {}

XFontFontDesc::~XFontFontDesc ()
{
  delete impl;
}

/*
   Количество шрифтов в наборе
*/

size_t XFontFontDesc::FontsCount ()
{
  return 1;
}

/*
   Имя гарнитуры / имя семейства / имя стиля
*/

const char* XFontFontDesc::FamilyName (size_t index)
{
  if (index > 1)
    throw xtl::make_range_exception ("media::xfont::XFontFontDesc::FamilyName", "index", index, 0u, 1u);

  return impl->font.FamilyName ();
}

const char* XFontFontDesc::StyleName (size_t index)
{
  if (index > 1)
    throw xtl::make_range_exception ("media::xfont::XFontFontDesc::StyleName", "index", index, 0u, 1u);

  return impl->font.StyleName ();
}

/*
   Создание шрифта
*/

Font XFontFontDesc::CreateFont (size_t index, const FontCreationParams& params)
{
  try
  {
    if (index > 1)
      throw xtl::make_range_exception ("", "index", index, 0u, 1u);

    if (!CanCreateFont (index, params))
      throw xtl::make_argument_exception ("", "params");

    return impl->font.Font ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::xfont::XFontFontDesc::CreateFont");
    throw;
  }
}

bool XFontFontDesc::CanCreateFont (size_t index, const FontCreationParams& params)
{
  if (index > 1)
    throw xtl::make_range_exception ("media::xfont::XFontFontDesc::CanCreateFont", "index", index, 0u, 1u);

  if (abs ((int)impl->font.FontSize () - (int)params.font_size) > params.font_size_eps)
    return false;

  return true;
}
