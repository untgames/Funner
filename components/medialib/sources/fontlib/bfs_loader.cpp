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

  if (!test (iter, "FontFile"))
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no 'FontFile' property");
  if (!test (iter, "FirstCharCode"))
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no 'FirstCharCode' property");
  if (!test (iter, "Glyphs"))
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no 'Glyphs' tag");

  read (iter, "Name", temp_name, "");
  read (iter, "FontFile", font_file);
  read (iter, "FirstCharCode", first_char_code);

  for (Parser::Iterator i = iter->First ("Glyphs.Glyph"); i; i++)
    glyph_count++;

  if (!glyph_count)
    Raise <Exception> ("FontFace::DefaultBFSLoader", "Incorrect file format, no glyphs");

  glyph_info   = new GlyphInfo [glyph_count];
  kerning_info = new KerningInfo [glyph_count * glyph_count];

  glyph_count = 0;
  for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; i++, glyph_count++)
  {
    if (!test (i, "XPos") || !test (i, "YPos") || !test (i, "Width") || !test (i, "Heigth"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      continue;
    }

    read (i, "XPos",     glyph_info[glyph_count].x_pos);
    read (i, "YPos",     glyph_info[glyph_count].y_pos);
    read (i, "Width",    glyph_info[glyph_count].width);
    read (i, "Heigth",   glyph_info[glyph_count].heigth);
    read (i, "BearingX", glyph_info[glyph_count].bearingX, 0);
    read (i, "BearingY", glyph_info[glyph_count].bearingY, (int)glyph_info[glyph_count].heigth);
    read (i, "AdvanceX", glyph_info[glyph_count].advanceX, (int)glyph_info[glyph_count].width);
    read (i, "AdvanceY", glyph_info[glyph_count].advanceY, 0);
  }

  for (size_t i = 0; i < glyph_count; i++)
    for (size_t j = 0; j < glyph_count; j++)
    {
      kerning_info [glyph_count * i + j].x_kerning = glyph_info[i].advanceX;
      kerning_info [glyph_count * i + j].y_kerning = glyph_info[i].advanceY;
    }

  for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; i++)
  {
    if (!test (i, "LeftGlyph") || !test (i, "RightGlyph") || !test (i, "XKerning") || !test (i, "YKerning"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      continue;
    }

    size_t left, right;

    read (i, "LeftGlyph",  left);
    read (i, "RightGlyph", right);
    read (i, "XKerning",   kerning_info[glyph_count * left + right].x_kerning);
    read (i, "YKerning",   kerning_info[glyph_count * left + right].y_kerning);
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
