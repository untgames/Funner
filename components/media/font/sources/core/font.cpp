#include "shared.h"

using namespace media;

/*
   Конструктор / деструктор / копирование
*/

Font::Font (FontImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Font::~Font ()
{
  release (impl);
}

Font::Font (const Font& source)
  : impl (source.impl)
{
  addref (impl);
}

Font& Font::operator = (const Font& source)
{
  Font (source).Swap (*this);

  return *this;
}

/*
   Имя файла
*/

const char* Font::Source () const
{
  return impl->Source ();
}

/*
   Имя гарнитуры / имя семейства / имя стиля
*/

const char* Font::Name () const
{
  return impl->Name ();
}

const char* Font::FamilyName () const
{
  return impl->FamilyName ();
}

const char* Font::StyleName () const
{
  return impl->StyleName ();
}

/*
   Размер таблицы глифов
*/

size_t Font::GlyphsCount () const
{
  return impl->GlyphsCount ();
}

/*
   Код первого глифа
*/

size_t Font::FirstGlyphCode () const
{
  return impl->FirstGlyphCode ();
}

/*
   Размер шрифта
*/

size_t Font::FontSize () const
{
  return impl->FontSize ();
}

/*
   Доступ к данным о глифах
*/

const GlyphInfo* Font::Glyphs () const
{
  return impl->Glyphs ();
}

/*
   Информация о кёрнингах
*/

KerningInfo Font::Kerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  return impl->Kerning (left_glyph_index, right_glyph_index);
}

bool Font::HasKerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  return impl->HasKerning (left_glyph_index, right_glyph_index);
}

/*
   Создание растеризованного шрифта
*/

RasterizedFont Font::CreateRasterizedFont (const RasterizedFontCreationParams& params) const
{
  return RasterizedFont (impl->Rasterizer () (*this, params));
}

/*
   Обмен
*/

void Font::Swap (Font& source)
{
  stl::swap (impl, source.impl);
}


namespace media
{

/*
   Обмен
*/

void swap (Font& font1, Font& font2)
{
  font1.Swap (font2);
}

}
