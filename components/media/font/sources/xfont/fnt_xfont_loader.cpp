#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

/*
    Загрузка шрифта
*/

namespace
{

void xfont_load (const char* file_name, Font& font)
{
  ParseLog         log;
  Parser           p (log, file_name);
  Parser::Iterator iter = p.Root ()->First("Font");
  stl::string      string_buffer;
  size_t           first_char_code, glyph_count = 0;
  GlyphInfo*       glyph_info;
  Font             new_font;

  static const char* METHOD_NAME = "media::xfont_load";

  if (iter)
  {
    for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; ++i, glyph_count++)
      if (!test (i, "XPos") || !test (i, "YPos") || !test (i, "Width") || !test (i, "Height"))
      {
        log.Error (i, "Incorrect file format, one of tag property missing");
        break;
      }

    for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; ++i)
      if (!test (i, "LeftGlyph") || !test (i, "RightGlyph") || !test (i, "XKerning") || !test (i, "YKerning"))
      {
        log.Error (i, "Incorrect file format, one of tag property missing");
        break;
      }
  }

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      throw xtl::format_operation_exception (METHOD_NAME, log.Message(i));

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Font' root tag");
    
  if (!glyph_count)
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no glyphs");

  if (!test (iter, "FontFile"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'FontFile' property");
    
  if (!test (iter, "FirstCharCode"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'FirstCharCode' property");
    
  if (!test (iter, "Glyphs"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Glyphs' tag");

  read (iter, "Name", string_buffer, "");
  new_font.Rename (string_buffer.c_str ());

  read (iter, "FontFile", string_buffer);
  new_font.SetImageName (string_buffer.c_str ());

  read (iter, "FirstCharCode", first_char_code);
  new_font.SetFirstGlyphCode (first_char_code);

  new_font.ResizeGlyphsTable (glyph_count);

  glyph_info = new_font.Glyphs ();

  glyph_count = 0;
  for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; ++i, glyph_count++)
  {
    read (i, "XPos",     glyph_info[glyph_count].x_pos);
    read (i, "YPos",     glyph_info[glyph_count].y_pos);
    read (i, "Width",    glyph_info[glyph_count].width);
    read (i, "Height",   glyph_info[glyph_count].height);
    read (i, "BearingX", glyph_info[glyph_count].bearing_x, 0);
    read (i, "BearingY", glyph_info[glyph_count].bearing_y, (int)glyph_info[glyph_count].height);
    read (i, "AdvanceX", glyph_info[glyph_count].advance_x, (int)glyph_info[glyph_count].width);
    read (i, "AdvanceY", glyph_info[glyph_count].advance_y, 0);
  }

  for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; ++i)
  {
    KerningInfo kerning_info;
    size_t      left, right;

    read (i, "LeftGlyph",  left);
    read (i, "RightGlyph", right);
    read (i, "XKerning",   kerning_info.x_kerning);
    read (i, "YKerning",   kerning_info.y_kerning);

    new_font.InsertKerning (left, right, kerning_info);
  }

  font.Swap (new_font);
}

/*
   Компонент загрузки шрифтов
*/

class XFontLoaderComponent
{
  public:
    //загрузка компонента
    XFontLoaderComponent () 
    {
      FontManager::RegisterLoader ("xfont", &xfont_load);
    }
};

extern "C"
{

ComponentRegistrator<XFontLoaderComponent> XFontLoader ("media.font.loaders.xfont");

}

}
