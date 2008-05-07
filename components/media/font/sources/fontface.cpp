#include <stl/string>
#include <stl/hash_set>
#include <stl/hash_map>
#include <stl/memory>

#include <xtl/function.h>

#include <common/singleton.h>
#include <common/exception.h>
#include <common/parser.h>

#include <media/font.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация гарнитуры
///////////////////////////////////////////////////////////////////////////////////////////////////
class FontFaceImpl
{
  public:
    FontFaceImpl () {glyphs_count = 0;}
    FontFaceImpl (size_t first_char, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning, const char* font_file_name);

    size_t                     first_char_index;  //char-код первого глифа
    size_t                     glyphs_count;      //количество глифов
    stl::auto_ptr<GlyphInfo>   glyphs;            //глифы
    stl::auto_ptr<KerningInfo> kerning_table;     //таблица межзнаковых интервалов
    stl::string                file_name;         //имя файла с содержимым гарнитуры
    stl::string                str_name;          //имя гарнитуры
};

};

namespace
{

const char* GetExtension (const char* file_name)
{
  const char*  ext;
  size_t len = strlen(file_name) - 1;
  
  if (file_name == NULL || !len)
    return file_name;

  ext = file_name + len;

  for (; len && (*ext != '.'); len--, ext--);

  if (!len)
    return file_name;

  return ext + 1;
}

}

using namespace common;
using namespace media;

FontFace::FontFace ()
  : impl (new FontFaceImpl)
{
}

FontFace::FontFace (size_t first_char_index, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning_table, const char* font_file_name)
  : impl (new FontFaceImpl (first_char_index, glyph_table_size, glyph, kerning_table, font_file_name))
{
  if (!glyph_table_size)
    RaiseNullArgument ("FontFace::FontFace", "glyph_table_size");
  if (!glyph)
    RaiseNullArgument ("FontFace::FontFace", "glyph");
}

FontFace::FontFace (const FontFace& source)
  : impl (source.impl)
{                  
}

FontFace::FontFace (const char* file_name)
{
  try
  {
    Load (file_name);
  }
  catch (Exception& exception)
  {
    exception.Touch ("media::FontFace::FontFace");
    throw;
  }
}

FontFace::FontFace (FontFaceImpl* source)
  : impl (source)
{
}

FontFace::~FontFace ()
{
}

FontFace& FontFace::operator = (const FontFace& source)
{
  if (this == &source)
    return *this;

  impl = source.impl;

  return *this; 
}

void FontFace::Load (const char* file_name)
{  
  ParseLog         log;
  Parser           p (log, file_name);
  Parser::Iterator iter = p.Root ()->First("Font");
  stl::string      temp_name, font_file;
  size_t           first_char_code, glyph_count = 0;
  GlyphInfo*       glyph_info = NULL;
  KerningInfo*     kerning_info = NULL;

  static const char* METHOD_NAME = "media::FontFace::Load";

  for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; i++, glyph_count++)
    if (!test (i, "XPos") || !test (i, "YPos") || !test (i, "Width") || !test (i, "Heigth"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      break;
    }

  for (Parser::NamesakeIterator i = iter->First ("Kernings.Kerning"); i; i++)
    if (!test (i, "LeftGlyph") || !test (i, "RightGlyph") || !test (i, "XKerning") || !test (i, "YKerning"))
    {
      log.Error (i, "Incorrect file format, one of tag property missing");
      break;
    }

  for (size_t i = 0; i < log.MessagesCount (); i++)
    if (log.MessageType (i) == PARSE_LOG_ERROR || log.MessageType (i) == PARSE_LOG_FATAL_ERROR)
      Raise <Exception> (METHOD_NAME, log.Message(i));

  if (!iter)
    Raise <Exception> (METHOD_NAME, "Incorrect file format, no 'Font' root tag");
  if (!test (iter, "FontFile"))
    Raise <Exception> (METHOD_NAME, "Incorrect file format, no 'FontFile' property");
  if (!test (iter, "FirstCharCode"))
    Raise <Exception> (METHOD_NAME, "Incorrect file format, no 'FirstCharCode' property");
  if (!test (iter, "Glyphs"))
    Raise <Exception> (METHOD_NAME, "Incorrect file format, no 'Glyphs' tag");

  read (iter, "Name", temp_name, "");
  read (iter, "FontFile", font_file);
  read (iter, "FirstCharCode", first_char_code);

  for (Parser::Iterator i = iter->First ("Glyphs.Glyph"); i; i++)
    glyph_count++;

  if (!glyph_count)
    Raise <Exception> (METHOD_NAME, "Incorrect file format, no glyphs");

  glyph_info   = new GlyphInfo [glyph_count];
  kerning_info = new KerningInfo [glyph_count * glyph_count];

  glyph_count = 0;
  for (Parser::NamesakeIterator i = iter->First ("Glyphs.Glyph"); i; i++, glyph_count++)
  {
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
    size_t left, right;

    read (i, "LeftGlyph",  left);
    read (i, "RightGlyph", right);
    read (i, "XKerning",   kerning_info[glyph_count * left + right].x_kerning);
    read (i, "YKerning",   kerning_info[glyph_count * left + right].y_kerning);
  }

  Swap (FontFace (first_char_code, glyph_count, glyph_info, kerning_info, font_file.c_str ()));
  impl->str_name = file_name;
}

const char* FontFace::Name () const
{
  return impl->str_name.c_str ();
}

void FontFace::Rename (const char* new_name)
{
  impl->str_name = new_name;
}

const char* FontFace::ImageName () const
{
  return impl->file_name.c_str ();
}

size_t FontFace::GlyphsCount () const
{
  return impl->glyphs_count;
}

size_t FontFace::FirstGlyphCode () const
{
  return impl->first_char_index;
}

GlyphInfo* FontFace::GlyphData (size_t glyph_index) const
{
  if (glyph_index >= impl->glyphs_count)
    RaiseOutOfRange ("FontFace::GlyphData", "glyph_index", glyph_index, 0u, impl->glyphs_count);
      
  return &(impl->glyphs.get()[glyph_index]);
}

GlyphInfo* FontFace::GlyphData () const
{
  return impl->glyphs.get();
}

KerningInfo* FontFace::Kerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  if (left_glyph_index >= impl->glyphs_count)
    RaiseOutOfRange ("FontFace::Kerning", "left_glyph_index", left_glyph_index, 0u, impl->glyphs_count);
  if (right_glyph_index >= impl->glyphs_count)
    RaiseOutOfRange ("FontFace::Kerning", "right_glyph_index", right_glyph_index, 0u, impl->glyphs_count);
      
  return &(impl->kerning_table.get()[left_glyph_index * impl->glyphs_count + right_glyph_index]);
}

KerningInfo* FontFace::Kerning () const
{
  return impl->kerning_table.get();
}

/*
   Сохранение
*/

void FontFace::Save (const char* file_name) const
{
  RaiseNotImplemented ("media::FontFace::Save");
}

void FontFace::Swap (FontFace& font)
{
  swap (impl, font.impl);
}

namespace media
{

void swap (FontFace& font1,FontFace& font2)
{
  font1.Swap (font2);
}

}

FontFaceImpl::FontFaceImpl (size_t first_char, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning, const char* font_file_name)
  : first_char_index (first_char), glyphs_count (glyph_table_size), glyphs (glyph), kerning_table (kerning), file_name (font_file_name)
{
}
