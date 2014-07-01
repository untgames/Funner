#include "shared.h"

using namespace media;

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация построителя шрифта
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FontBuilder::Impl : public xtl::reference_counter
{
  Impl ()
    : font_impl (new FontImpl)
    {}

  ~Impl ()
  {
    release (font_impl);
  }

  FontImpl* font_impl;  //шрифт
};

}

/*
   Конструктор / деструктор / копирование
*/

FontBuilder::FontBuilder ()
  : impl (new Impl)
{
}

FontBuilder::~FontBuilder ()
{
  release (impl);
}

FontBuilder::FontBuilder (const FontBuilder& source)
  : impl (source.impl)
{
  addref (impl);
}

FontBuilder& FontBuilder::operator = (const FontBuilder& source)
{
  FontBuilder (source).Swap (*this);

  return *this;
}

/*
   Получение / изменение имени исходного файла
*/

const char* FontBuilder::Source () const
{
  return impl->font_impl->Source ();
}

void FontBuilder::SetSource (const char* new_source)
{
  if (!new_source)
    throw xtl::make_null_argument_exception ("media::FontBuilder::SetSource", "new_source");

  impl->font_impl->SetSource (new_source);
}

/*
   Получение/изменение имени / семейства / стиля шрифта
*/

const char* FontBuilder::Name () const
{
  return impl->font_impl->Name ();
}

const char* FontBuilder::FamilyName () const
{
  return impl->font_impl->FamilyName ();
}

const char* FontBuilder::StyleName () const
{
  return impl->font_impl->StyleName ();
}

void FontBuilder::Rename (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("media::FontBuilder::Rename", "new_name");

  impl->font_impl->Rename (new_name);
}

void FontBuilder::SetFamilyName (const char* new_family_name)
{
  if (!new_family_name)
    throw xtl::make_null_argument_exception ("media::FontBuilder::SetFamilyName", "new_family_name");

  impl->font_impl->SetFamilyName (new_family_name);
}

void FontBuilder::SetStyleName (const char* new_style_name)
{
  if (!new_style_name)
    throw xtl::make_null_argument_exception ("media::FontBuilder::SetStyleName", "new_style_name");

  impl->font_impl->SetStyleName (new_style_name);
}

/*
   Получение/изменение размера таблицы глифов
*/

void FontBuilder::SetGlyphsCount (size_t new_glyphs_count)
{
  impl->font_impl->SetGlyphsCount (new_glyphs_count);
}

size_t FontBuilder::GlyphsCount () const
{
  return impl->font_impl->GlyphsCount ();
}

/*
   Получение/изменение кода первого глифа
*/

void FontBuilder::SetFirstGlyphCode (size_t new_first_glyph_code)
{
  impl->font_impl->SetFirstGlyphCode (new_first_glyph_code);
}

size_t FontBuilder::FirstGlyphCode () const
{
  return impl->font_impl->FirstGlyphCode ();
}

/*
   Получение/изменение размера шрифта
*/

void FontBuilder::SetFontSize (size_t new_font_size)
{
  impl->font_impl->SetFontSize (new_font_size);
}

size_t FontBuilder::FontSize () const
{
  return impl->font_impl->FontSize ();
}

/*
   Доступ к данным о глифах
*/

const GlyphInfo* FontBuilder::Glyphs () const
{
  return impl->font_impl->Glyphs ();
}

GlyphInfo* FontBuilder::Glyphs ()
{
  return const_cast<GlyphInfo*> (const_cast<const FontBuilder&> (*this).Glyphs ());
}

/*
   Добавление/получение/удаление информации о кёрнингах
*/

void FontBuilder::InsertKerning (size_t left_glyph_index, size_t right_glyph_index, const KerningInfo& kerning_info)
{
  impl->font_impl->InsertKerning (left_glyph_index, right_glyph_index, kerning_info);
}

void FontBuilder::RemoveKerning (size_t left_glyph_index, size_t right_glyph_index)
{
  impl->font_impl->RemoveKerning (left_glyph_index, right_glyph_index);
}

void FontBuilder::RemoveAllKernings ()
{
  impl->font_impl->RemoveAllKernings ();
}

KerningInfo FontBuilder::Kerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  return impl->font_impl->Kerning (left_glyph_index, right_glyph_index);
}

bool FontBuilder::HasKerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  return impl->font_impl->HasKerning (left_glyph_index, right_glyph_index);
}


/*
   Установка растеризатора
*/

void FontBuilder::SetRasterizer (const FontBuilder::RasterizerHandler& new_rasterizer)
{
  impl->font_impl->SetRasterizer (new_rasterizer);
}

const FontBuilder::RasterizerHandler& FontBuilder::Rasterizer () const
{
  return impl->font_impl->Rasterizer ();
}

/*
   Получение шрифта
*/

media::Font FontBuilder::Font ()
{
  return media::Font (impl->font_impl);
}

/*
   Обмен
*/

void FontBuilder::Swap (FontBuilder& source)
{
  stl::swap (impl, source.impl);
}


namespace media
{

/*
   Обмен
*/

void swap (FontBuilder& builder1, FontBuilder& builder2)
{
  builder1.Swap (builder2);
}

}
