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
  Parser           p (file_name);
  ParseLog         log = p.Log ();
  Parser::Iterator iter = p.Root ().First("Font");
  size_t           first_char_code = 0, glyph_count = 0;
  GlyphInfo*       glyph_info;
  Font             new_font;

  static const char* METHOD_NAME = "media::xfont_load";

  if (iter)
  {
    for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; ++i, glyph_count++)
      if (!i->First ("XPos") || !i->First ("YPos") || !i->First ("Width") || !i->First ("Height"))
      {
        log.Error (*i, "Incorrect file format, one of tag property missing");
        break;
      }

    for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; ++i)
      if (!i->First ("LeftGlyph") || !i->First ("RightGlyph") || !i->First ("XKerning") || !i->First ("YKerning"))
      {
        log.Error (*i, "Incorrect file format, one of tag property missing");
        break;
      }
  }

  for (size_t i = 0; i < log.MessagesCount (); i++)
    switch (log.MessageType (i))
    {
      case ParseLogMessageType_Error:
      case ParseLogMessageType_FatalError:
        throw xtl::format_operation_exception (METHOD_NAME, log.Message (i));
      default:
        break;
    }

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Font' root tag");

  if (!glyph_count)
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no glyphs");

  if (!iter->First ("FontFile"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'FontFile' property");

  if (!iter->First ("FirstCharCode"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'FirstCharCode' property");

  if (!iter->First ("Glyphs"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Glyphs' tag");

  stl::string string_buffer;

  read (*iter, "Name", string_buffer);

  new_font.Rename (string_buffer.c_str ());

  read (*iter, "FontFile", string_buffer);

  new_font.SetImageName (string_buffer.c_str ());

  read (*iter, "FirstCharCode", first_char_code);

  new_font.SetFirstGlyphCode (first_char_code);

  new_font.ResizeGlyphsTable (glyph_count);

  glyph_info = new_font.Glyphs ();

  glyph_count = 0;
  for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; ++i, glyph_count++)
  {
    GlyphInfo& glyph = glyph_info [glyph_count];

    memset (&glyph, 0, sizeof glyph);

    try_read (*i, "XPos",     glyph.x_pos);
    try_read (*i, "YPos",     glyph.y_pos);
    try_read (*i, "Width",    glyph.width);
    try_read (*i, "Height",   glyph.height);

    glyph.bearing_x = (int)glyph.height;
    glyph.advance_x = (int)glyph.width;

    try_read (*i, "BearingX", glyph.bearing_x);
    try_read (*i, "BearingY", glyph.bearing_y);
    try_read (*i, "AdvanceX", glyph.advance_x);
    try_read (*i, "AdvanceY", glyph.advance_y);
  }

  for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; ++i)
  {
    KerningInfo kerning_info;
    size_t      left = 0, right = 0;

    memset (&kerning_info, 0, sizeof kerning_info);

    try_read (*i, "LeftGlyph",  left);
    try_read (*i, "RightGlyph", right);
    try_read (*i, "XKerning",   kerning_info.x_kerning);
    try_read (*i, "YKerning",   kerning_info.y_kerning);

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
