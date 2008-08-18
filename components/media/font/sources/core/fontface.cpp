#include <stl/string>
#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/uninitialized_storage.h>

#include <common/component.h>
#include <common/hash.h>

#include <media/font.h>

using namespace common;
using namespace media;

namespace
{

struct KerningsMapKey
{
  KerningsMapKey (size_t in_left_glyph_index, size_t in_right_glyph_index)
    : left_glyph_index (in_left_glyph_index), right_glyph_index (in_right_glyph_index)
    {}

  bool operator == (const KerningsMapKey& right) const
  {
    return (left_glyph_index == right.left_glyph_index) && (right_glyph_index == right.right_glyph_index);
  }

  size_t left_glyph_index;
  size_t right_glyph_index;
};

size_t hash (const KerningsMapKey& key)
{
  size_t data_to_hash [2] = {key.left_glyph_index, key.right_glyph_index};

  return crc32 (data_to_hash, sizeof (data_to_hash));
}

}

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация гарнитуры
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Font::Impl : public xtl::reference_counter
{
  Impl () : first_char_index (0) {}

  typedef xtl::uninitialized_storage<GlyphInfo>      GlyphsArray;
  typedef stl::hash_map<KerningsMapKey, KerningInfo> KerningsMap;

  size_t      first_char_index;  //char-код первого глифа
  GlyphsArray glyphs;            //глифы
  stl::string file_name;         //имя файла с содержимым гарнитуры
  stl::string str_name;          //имя гарнитуры
  KerningsMap kernings;          //кернинги
};

}

Font::Font ()
  : impl (new Impl)
{
}

Font::Font (const Font& source)
  : impl (source.impl)
{                  
  addref (impl);
}

Font::Font (const char* file_name)
  : impl (new Impl)
{
  static const char* METHOD_NAME = "media::Font::Font";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  try
  {
    static ComponentLoader loader ("media.font.loaders.*");

    FontManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

Font::~Font ()
{
  release (impl);
}

Font& Font::operator = (const Font& source)
{
  if (this == &source)
    return *this;

  impl = source.impl;

  return *this; 
}

/*
   Загрузка
*/

void Font::Load (const char* file_name)
{
  try
  {
    Font (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::Font::Load");
    throw;
  }
}

/*
   Получение/изменение имени шрифта
*/

const char* Font::Name () const
{
  return impl->str_name.c_str ();
}

void Font::Rename (const char* new_name)
{
  if (!new_name)
    throw xtl::make_null_argument_exception ("media::Font::Rename", "new_name");

  impl->str_name = new_name;
}

/*
   Получение/изменение имени файла текстуры шрифта
*/

void Font::SetImageName (const char* new_image_name)
{
  if (!new_image_name)
    throw xtl::make_null_argument_exception ("media::Font::SetImageName", "new_image_name");

  impl->file_name = new_image_name;
}

const char* Font::ImageName () const
{
  return impl->file_name.c_str ();
}

/*
   Получение/изменение размера таблицы глифов 
*/

void Font::ResizeGlyphsTable (size_t new_glyphs_count)
{
  impl->glyphs.resize (new_glyphs_count);
}

size_t Font::GlyphsTableSize () const
{
  return impl->glyphs.size ();
}

/*
   Получение/изменение кода первого глифа
*/

void Font::SetFirstGlyphCode (size_t new_first_glyph_code)
{
  impl->first_char_index = new_first_glyph_code;
}

size_t Font::FirstGlyphCode () const
{
  return impl->first_char_index;
}

/*
   Доступ к данным о глифах
*/

const GlyphInfo* Font::Glyphs () const
{
  return impl->glyphs.data ();
}

GlyphInfo* Font::Glyphs ()
{
  return const_cast<GlyphInfo*> (const_cast<const Font&> (*this).Glyphs ());
}

/*
   Добавление/получение/удаление информации о кёрнингах
*/

void Font::InsertKerning (size_t left_glyph_index, size_t right_glyph_index, const KerningInfo& kerning_info)
{
  impl->kernings[KerningsMapKey (left_glyph_index, right_glyph_index)] = kerning_info;
}

void Font::RemoveKerning (size_t left_glyph_index, size_t right_glyph_index)
{
  impl->kernings.erase (KerningsMapKey (left_glyph_index, right_glyph_index));
}

void Font::RemoveAllKernings ()
{
  impl->kernings.clear ();
}

KerningInfo Font::Kerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  Impl::KerningsMap::iterator iter = impl->kernings.find (KerningsMapKey (left_glyph_index, right_glyph_index));

  if (iter == impl->kernings.end ())
  {
    KerningInfo return_value = {0.f, 0.f};

    return return_value;
  }

  return iter->second;
}

bool Font::HasKerning (size_t left_glyph_index, size_t right_glyph_index) const
{
  Impl::KerningsMap::iterator iter = impl->kernings.find (KerningsMapKey (left_glyph_index, right_glyph_index));

  if (iter == impl->kernings.end ())
    return false;

  return true;
}

/*
   Сохранение
*/

void Font::Save (const char* file_name) const
{
  static const char* METHOD_NAME = "media::Font::Save";

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");
    
  try
  {
    static ComponentLoader loader ("media.font.savers.*");

    FontManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

void Font::Swap (Font& font)
{
  stl::swap (impl, font.impl);
}

namespace media
{

void swap (Font& font1,Font& font2)
{
  font1.Swap (font2);
}

}
