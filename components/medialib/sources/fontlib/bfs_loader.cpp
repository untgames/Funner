#include <common/parser.h>
#include <common/exception.h>
#include "shared.h"

using namespace medialib;
using namespace common;

namespace medialib
{

FontFace* FontFace::DefaultBFSLoader (const char* file_name)
{
  ParseLog         log;
  Parser           p (log, file_name);
  Parser::Iterator iter = p.Root ()->First("Font");
  stl::string      temp_name, font_file;
  size_t           first_char_code, glyph_count = 0;
  GlyphInfo*       glyph_info = NULL;
  KerningInfo*     kerning_info = NULL;

  if (!iter->Present ("FontFile"))
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no 'FontFile' property");
  if (!iter->Present ("FirstCharCode"))
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no 'FirstCharCode' property");
  if (!iter->Present ("Glyphs"))
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no 'Glyphs' tag");

  iter->Read ("Name", temp_name, "");
  iter->Read ("FontFile", font_file);
  iter->Read ("FirstCharCode", first_char_code);

  for (Parser::Iterator i = iter->First ("Glyphs.Glyph"); i; i++)
    glyph_count++;

  if (!glyph_count)
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no glyphs");

  glyph_info   = new GlyphInfo [glyph_count];
  kerning_info = new KerningInfo [glyph_count * glyph_count];

  glyph_count = 0;
  for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; i++, glyph_count++)
  {
    if (!i->Present ("XPos") || !i->Present("YPos") || !i->Present("Width") || !i->Present("Heigth"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      continue;
    }

    i->Read ("XPos",     glyph_info[glyph_count].x_pos);
    i->Read ("YPos",     glyph_info[glyph_count].y_pos);
    i->Read ("Width",    glyph_info[glyph_count].width);
    i->Read ("Heigth",   glyph_info[glyph_count].heigth);
    i->Read ("BearingX", glyph_info[glyph_count].bearingX, 0);
    i->Read ("BearingY", glyph_info[glyph_count].bearingY, (int)glyph_info[glyph_count].heigth);
    i->Read ("AdvanceX", glyph_info[glyph_count].advanceX, (int)glyph_info[glyph_count].width);
    i->Read ("AdvanceY", glyph_info[glyph_count].advanceY, 0);
  }

  for (size_t i = 0; i < glyph_count; i++)
    for (size_t j = 0; j < glyph_count; j++)
    {
      kerning_info [glyph_count * i + j].x_kerning = glyph_info[i].advanceX;
      kerning_info [glyph_count * i + j].y_kerning = glyph_info[i].advanceY;
    }

  for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; i++)
  {
    if (!i->Present ("LeftGlyph") || !i->Present("RightGlyph") || !i->Present("XKerning") || !i->Present("YKerning"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      continue;
    }

    size_t left, right;

    i->Read ("LeftGlyph",  left);
    i->Read ("RightGlyph", right);
    i->Read ("XKerning",   kerning_info[glyph_count * left + right].x_kerning);
    i->Read ("YKerning",   kerning_info[glyph_count * left + right].y_kerning);
  }

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
    {

      delete [] glyph_info;
      delete [] kerning_info;
      Raise <Exception> ("FontFace::DefaultBFSLoader", log.Message(0));

      return new FontFace ();
    }

  return new FontFace (first_char_code, glyph_count, glyph_info, kerning_info, font_file.c_str ());
}

}
