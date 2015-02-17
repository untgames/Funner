#include "shared.h"

using namespace media;

namespace
{

struct KerningsMapKey
{
  KerningsMapKey (unsigned int in_left_glyph_index, unsigned int in_right_glyph_index)
    : left_glyph_index (in_left_glyph_index), right_glyph_index (in_right_glyph_index)
    {}

  bool operator == (const KerningsMapKey& right) const
  {
    return (left_glyph_index == right.left_glyph_index) && (right_glyph_index == right.right_glyph_index);
  }

  unsigned int left_glyph_index;
  unsigned int right_glyph_index;
};

size_t hash (const KerningsMapKey& key)
{
  unsigned int data_to_hash [2] = {key.left_glyph_index, key.right_glyph_index};

  return common::crc32 (data_to_hash, sizeof (data_to_hash));
}

}

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация гарнитуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FontImpl::Impl
{
  Impl ()
    : first_glyph_code (0)
    , font_size (0)
    {}

  typedef xtl::uninitialized_storage<GlyphInfo>      GlyphsArray;
  typedef stl::hash_map<KerningsMapKey, KerningInfo> KerningsMap;

  unsigned int                   first_glyph_code;  //char-код первоunsigned intфа
  unsigned int                   font_size;         //размер шрифта
  stl::string                    file_name;         //имя исходного файла
  stl::string                    name;              //имя гарнитуры
  stl::string                    family_name;       //имя семейства гарнитуры
  stl::string                    style_name;        //имя стиля гарнитуры
  GlyphsArray                    glyphs;            //глифы
  KerningsMap                    kernings;          //кернинги
  FontBuilder::RasterizerHandler rasterizer;        //растеризатор
};

}

/*
   Конструктор / деструктор
*/

FontImpl::FontImpl ()
  : impl (new Impl)
{
}

FontImpl::~FontImpl ()
{
}

/*
   Получение / изменение имени исходного файла
*/

const char* FontImpl::Source () const
{
  return impl->file_name.c_str ();
}

void FontImpl::SetSource (const char* new_source)
{
  if (!new_source)
    throw xtl::make_null_argument_exception ("media::FontImpl::SetSource", "new_source");

  impl->file_name = new_source;
}

/*
   Получение/изменение имени / семейства / стиля шрифта
*/

const char* FontImpl::Name () const
{
  return impl->name.c_str ();
}

const char* FontImpl::FamilyName () const
{
  return impl->family_name.c_str ();
}

const char* FontImpl::StyleName () const
{
  return impl->style_name.c_str ();
}

void FontImpl::Rename (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("media::FontImpl::Rename", "new_name");

  impl->name = new_name;
}

void FontImpl::SetFamilyName (const char* new_family_name)
{
  if (!new_family_name)
    throw xtl::make_null_argument_exception ("media::FontImpl::SetFamilyName", "new_family_name");

  impl->family_name = new_family_name;
}

void FontImpl::SetStyleName (const char* new_style_name)
{
  if (!new_style_name)
    throw xtl::make_null_argument_exception ("media::FontImpl::SetStyleName", "new_style_name");

  impl->style_name = new_style_name;
}

/*
   Получение/изменение размера таблицы глифов
*/

void FontImpl::SetGlyphsCount (unsigned int new_glyphs_count)
{
  impl->glyphs.resize (new_glyphs_count);
}

unsigned int FontImpl::GlyphsCount () const
{
  return (unsigned int)impl->glyphs.size ();
}

/*
   Получение/изменение кода первого глифа
*/

void FontImpl::SetFirstGlyphCode (unsigned int new_first_glyph_code)
{
  impl->first_glyph_code = new_first_glyph_code;
}

unsigned int FontImpl::FirstGlyphCode () const
{
  return impl->first_glyph_code;
}

/*
   Получение/изменение размера шрифта
*/

void FontImpl::SetFontSize (unsigned int new_font_size)
{
  impl->font_size = new_font_size;
}

unsigned int FontImpl::FontSize () const
{
  return impl->font_size;
}

/*
   Доступ к данным о глифах
*/

const GlyphInfo* FontImpl::Glyphs () const
{
  return impl->glyphs.data ();
}

GlyphInfo* FontImpl::Glyphs ()
{
  return const_cast<GlyphInfo*> (const_cast<const FontImpl&> (*this).Glyphs ());
}

/*
   Добавление/получение/удаление информации о кёрнингах
*/

void FontImpl::InsertKerning (unsigned int left_glyph_index, unsigned int right_glyph_index, const KerningInfo& kerning_info)
{
  impl->kernings[KerningsMapKey (left_glyph_index, right_glyph_index)] = kerning_info;
}

void FontImpl::RemoveKerning (unsigned int left_glyph_index, unsigned int right_glyph_index)
{
  impl->kernings.erase (KerningsMapKey (left_glyph_index, right_glyph_index));
}

void FontImpl::RemoveAllKernings ()
{
  impl->kernings.clear ();
}

KerningInfo FontImpl::Kerning (unsigned int left_glyph_index, unsigned int right_glyph_index) const
{
  Impl::KerningsMap::iterator iter = impl->kernings.find (KerningsMapKey (left_glyph_index, right_glyph_index));

  if (iter == impl->kernings.end ())
  {
    KerningInfo return_value = {0.f, 0.f};

    return return_value;
  }

  return iter->second;
}

bool FontImpl::HasKerning (unsigned int left_glyph_index, unsigned int right_glyph_index) const
{
  Impl::KerningsMap::iterator iter = impl->kernings.find (KerningsMapKey (left_glyph_index, right_glyph_index));

  if (iter == impl->kernings.end ())
    return false;

  return true;
}


/*
   Установка растеризатора
*/

void FontImpl::SetRasterizer (const FontBuilder::RasterizerHandler& new_rasterizer)
{
  impl->rasterizer = new_rasterizer;
}

const FontBuilder::RasterizerHandler& FontImpl::Rasterizer () const
{
  return impl->rasterizer;
}
