#include "shared.h"

using namespace media;
using namespace common;

namespace components
{

namespace xfont_saver
{

/*
    Константы
*/

const char* FLOAT_FORMAT = ".000"; //количество знаков после запятой при выводе вещественных чисел

/*
    Вспомогательный класс сохранения шрифтов
*/

class XmlFontSaver
{
  public:
    /*
        Конструктор
    */

    XmlFontSaver (const char* file_name, const Font& font, const char* image_name_format, const RasterizedFontCreationParams& params)
      : writer (file_name)
    {
      SaveFont (font, image_name_format, params);
    }

  private:
    /*
        Сохранение шрифта
    */

    void SaveFont (const Font& font, const char* image_name_format, const RasterizedFontCreationParams& params)
    {
      RasterizedFont rasterized_font (font.CreateRasterizedFont (params));

      XmlWriter::Scope scope (writer, "Font");
      
      writer.WriteAttribute ("Name", font.Name ());
      writer.WriteAttribute ("FamilyName", font.FamilyName ());
      writer.WriteAttribute ("StyleName", font.StyleName ());
      writer.WriteAttribute ("FirstCharCode", font.FirstGlyphCode ());
      writer.WriteAttribute ("FontSize", font.FontSize ());
      writer.WriteAttribute ("FontFile", image_name_format);
      writer.WriteAttribute ("ImagesCount", rasterized_font.ImagesCount ());

      for (size_t i = 0, count = rasterized_font.ImagesCount (); i < count; i++)
      {
        media::Image image;

        rasterized_font.BuildImage (i, image);

        image.Save (common::format (image_name_format, i).c_str ());
      }

      SaveGlyphs (font, rasterized_font);
      SaveKernings (font);
    }

    /*
        Сохранение глифов
    */

    void SaveGlyphs (const Font& font, const RasterizedFont& rasterized_font)
    {
      XmlWriter::Scope scope (writer, "Glyphs");

      const GlyphInfo*           current_glyph            = font.Glyphs ();
      const RasterizedGlyphInfo* current_rasterized_glyph = rasterized_font.RasterizedGlyphs ();

      for (size_t i = 0, count = font.GlyphsCount (); i < count; i++, current_glyph++, current_rasterized_glyph++)
        SaveGlyph (*current_glyph, *current_rasterized_glyph);
    }

    void SaveGlyph (const GlyphInfo& glyph_info, const RasterizedGlyphInfo& rasterized_glyph_info)
    {
      XmlWriter::Scope scope (writer, "Glyph");

      writer.WriteAttribute ("ImageIndex", rasterized_glyph_info.image_index);
      writer.WriteAttribute ("XPos", rasterized_glyph_info.x_pos);
      writer.WriteAttribute ("YPos", rasterized_glyph_info.y_pos);
      writer.WriteAttribute ("Width", rasterized_glyph_info.width);
      writer.WriteAttribute ("Height", rasterized_glyph_info.height);
      writer.WriteAttribute ("BearingX", glyph_info.bearing_x, FLOAT_FORMAT);
      writer.WriteAttribute ("BearingY", glyph_info.bearing_y, FLOAT_FORMAT);
      writer.WriteAttribute ("AdvanceX", glyph_info.advance_x, FLOAT_FORMAT);
      writer.WriteAttribute ("AdvanceY", glyph_info.advance_y, FLOAT_FORMAT);
    }

    /*
        Сохранение кернингов
    */

    void SaveKernings (const Font& font)
    {
      XmlWriter::Scope scope (writer, "Kernings");
      
      for (size_t i = 0, count = font.GlyphsCount (); i < count; i++)
        for (size_t j = 0; j < count; j++)
          if (font.HasKerning (i, j))
            SaveKerning (font, i, j);
    }

    void SaveKerning (const Font& font, size_t left_glyph_index, size_t right_glyph_index)
    {
      XmlWriter::Scope scope (writer, "Kerning");
      KerningInfo      kerning_info (font.Kerning (left_glyph_index, right_glyph_index));

      writer.WriteAttribute ("LeftGlyph", left_glyph_index);
      writer.WriteAttribute ("RightGlyph", right_glyph_index);
      writer.WriteAttribute ("XKerning", kerning_info.x_kerning, FLOAT_FORMAT);
      writer.WriteAttribute ("YKerning", kerning_info.y_kerning, FLOAT_FORMAT);
    }
    
  private:
    XmlWriter writer; //сериализатор XML
};

/*
    Сохранение шрифта
*/

void xfont_save (const char* file_name, const Font& font, const char* image_name_format, const RasterizedFontCreationParams& params)
{
  try
  {
    XmlFontSaver (file_name, font, image_name_format, params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::xfont_save");
    throw;
  }
}

/*
   Компонент сохранение шрифтов
*/

class XFontSaverComponent
{
  public:
    //загрузка компонента
    XFontSaverComponent () 
    {
      FontManager::RegisterSaver ("xfont", &xfont_save);
    }
};

extern "C"
{

ComponentRegistrator<XFontSaverComponent> XFontSaver ("media.font.savers.xfont");

}

}

}
