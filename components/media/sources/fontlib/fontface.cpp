#include <common/exception.h>
#include "shared.h"

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
  FontSystemSingleton::Instance ().RegisterFontFace (*this);
}

FontFace::FontFace (size_t first_char_index, size_t glyph_table_size, GlyphInfo* glyph, KerningInfo* kerning_table, const char* font_file_name)
  : impl (new FontFaceImpl (first_char_index, glyph_table_size, glyph, kerning_table, font_file_name))
{
  if (!glyph_table_size)
    RaiseNullArgument ("FontFace::FontFace", "glyph_table_size");
  if (!glyph)
    RaiseNullArgument ("FontFace::FontFace", "glyph");

  FontSystemSingleton::Instance ().RegisterFontFace (*this);
}

FontFace::FontFace (const FontFace& source)
  : impl (source.impl)
{                  
  FontSystemSingleton::Instance ().RegisterFontFace (*this);
}

FontFace::FontFace (const char* file_name)
{
  Load (file_name);
  
  FontSystemSingleton::Instance ().RegisterFontFace (*this);
}

FontFace::FontFace (FontFaceImpl* source)
  : impl (source)
{
  FontSystemSingleton::Instance ().RegisterFontFace (*this);
}

FontFace::~FontFace ()
{
  FontSystemSingleton::Instance ().UnregisterFontFace (*this);
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
  FontSystem::CodecLoadFunc* load_func = FontSystemSingleton::Instance ().GetLoadFunc(GetExtension (file_name));

  if (!load_func)
    Raise <Exception> ("FontFace::FontFace", "Can't create font face from file '%s'. Format not supported.", file_name);

  impl = ((*load_func) (file_name))->impl;
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
