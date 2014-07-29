#include "shared.h"

using namespace media;

/*
   Библиотека шрифтов
*/

namespace
{

const char* FONT_LOADERS_MASK = "media.font.loaders.*"; //маска имён компонентов загрузки шрифтов
const char* LOG_NAME          = "media.FontLibrary";    //имя протокола

//Элемент карты загруженных шрифтов
struct FontEntry : public xtl::reference_counter
{
  stl::string font_name; //имя шрифта
  FontDesc    font_desc; //описание шрифта

  FontEntry (const FontDesc& in_font_desc)
    : font_desc (in_font_desc)
  {
    font_name = common::format ("%s %s", font_desc.FamilyName (), font_desc.StyleName ());
  }
};

typedef xtl::intrusive_ptr<FontEntry> FontEntryPtr;
typedef stl::vector<FontEntryPtr>     FontsArray;

struct FontsArraySelector
{
  FontDesc& operator () (FontsArray::value_type& value) const { return value->font_desc; }
};

bool find_font_less_comparator (const FontEntryPtr& font, const char* name)
{
  return font->font_name < name;
}

bool find_font_less_comparator_ptr (const FontEntryPtr& font1, const FontEntryPtr& font2)
{
  return font1->font_name < font2->font_name;
}

bool remove_by_file_name (const FontEntryPtr& font, const char* file_name)
{
  return !xtl::xstrcmp (font->font_desc.Source (), file_name);
}

bool remove_by_wildcard (const FontEntryPtr& font, const char* wildcard)
{
  return common::wcmatch (font->font_desc.Source (), wildcard);
}

}

namespace media
{

struct FontLibrary::Impl : public xtl::reference_counter
{
  stl::string name;        //имя библиотеки
  stl::string cache_dir;   //путь к папке хранения кеша
  bool        cache_state; //включено ли кеширование
  FontsArray  fonts;       //загруженные шрифты

  Impl ()
    : cache_state (true)
    {}

  ///Загрузка шрифта
  void LoadFont (const char* file_name, const FontManager::LoadHandler* load_handler)
  {
    xtl::com_ptr<IFontDesc> font_desc ((*load_handler) (file_name), false);

    FontsArray loaded_fonts;

    loaded_fonts.reserve (font_desc->FontsCount ());

    for (size_t i = 0, count = font_desc->FontsCount (); i < count; i++)
    {
      loaded_fonts.push_back (FontEntryPtr (new FontEntry (FontDesc (file_name, font_desc.get (), i)), false));
    }

    fonts.insert (fonts.end (), loaded_fonts.begin (), loaded_fonts.end ());
  }
};

}

/*
   Конструкторы / деструктор / присваивание
*/

FontLibrary::FontLibrary ()
  : impl (new Impl)
{
}

FontLibrary::FontLibrary (const FontLibrary& source)
  : impl (source.impl)
{
  addref (impl);
}

FontLibrary::~FontLibrary ()
{
  release (impl);
}

FontLibrary& FontLibrary::operator = (const FontLibrary& source)
{
  FontLibrary (source).Swap (*this);

  return *this;
}

/*
    Идентификатор
*/

size_t FontLibrary::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
   Имя библиотеки
*/

const char* FontLibrary::Name () const
{
  return impl->name.c_str ();
}

void FontLibrary::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::FontLibrary::SetName", "name");

  impl->name = name;
}

/*
   Количество дескрипторов шрифтов в библиотеке / проверка на пустоту
*/
size_t FontLibrary::Size () const
{
  return impl->fonts.size ();
}

bool FontLibrary::IsEmpty () const
{
  return impl->fonts.empty ();
}

/*
   Получение итератора
*/

FontLibrary::Iterator FontLibrary::CreateIterator ()
{
  return Iterator (impl->fonts.begin (), impl->fonts.begin (), impl->fonts.end (), FontsArraySelector ());
}

FontLibrary::ConstIterator FontLibrary::CreateIterator () const
{
  return ConstIterator (impl->fonts.begin (), impl->fonts.begin (), impl->fonts.end (), FontsArraySelector ());
}

/*
   Получение идентификатора шрифта
*/

const char* FontLibrary::FontName (const ConstIterator& i) const
{
  const FontsArray::const_iterator* citer = i.target<FontsArray::const_iterator> ();

  if (citer)
    return (**citer)->font_name.c_str ();

  const FontsArray::iterator* iter = i.target<FontsArray::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::FontLibrary::FontName", "iterator", "wrong-type");

  return (**iter)->font_name.c_str ();
}

/*
   Поиск
*/

FontDesc* FontLibrary::Find (const char* name) const
{
  if (!name)
    return 0;

  FontsArray::iterator iter = stl::lower_bound (impl->fonts.begin (), impl->fonts.end (), name, find_font_less_comparator);

  if (iter == impl->fonts.end ())
  {
    if (impl->fonts.empty ())
      return 0;

    return &impl->fonts [0]->font_desc;
  }

  return &(*iter)->font_desc;
}

/*
   Очистка библиотеки
*/

void FontLibrary::Clear ()
{
  impl->fonts.clear ();
}

/*
   Загрузка шрифта
*/

void FontLibrary::LoadFont (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    static common::ComponentLoader components_loader (FONT_LOADERS_MASK);

     const FontManager::LoadHandler& loader = FontManager::GetLoader (file_name, common::SerializerFindMode_ByName);

    impl->LoadFont (file_name, &loader);

    stl::sort (impl->fonts.begin (), impl->fonts.end (), find_font_less_comparator_ptr);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::FontLibrary::LoadFont ('%s')", file_name ? file_name : "(null)");
    throw;
  }
}

void FontLibrary::LoadFonts (const char* wildcard)
{

  try
  {
    if (!wildcard)
      throw xtl::make_null_argument_exception ("", "wildcard");

    common::FileList file_list = common::FileSystem::Search (wildcard, common::FileSearch_Files);

    static common::ComponentLoader components_loader (FONT_LOADERS_MASK);

    for (common::FileListIterator iter = file_list.GetIterator (); iter; ++iter)
    {
      const FontManager::LoadHandler *loader = FontManager::FindLoader (iter->name, common::SerializerFindMode_ByName);

      if (!loader)
        continue;

      try
      {
        impl->LoadFont (iter->name, loader);
      }
      catch (xtl::exception& e)
      {
        common::Log (LOG_NAME).Printf ("media::FontLibrary::LoadFonts: exception while loading font '%s': '%s'", iter->name, e.what ());
      }
      catch (...)
      {
        common::Log (LOG_NAME).Printf ("media::FontLibrary::LoadFonts: unknown exception while loading font '%s'", iter->name);
      }
    }

    stl::sort (impl->fonts.begin (), impl->fonts.end (), find_font_less_comparator_ptr);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::FontLibrary::LoadFonts ('%s')", wildcard ? wildcard : "(null)");
    throw;
  }
}

void FontLibrary::UnloadFont (const char* file_name)
{
  if (!file_name)
    return;

  impl->fonts.erase (stl::remove_if (impl->fonts.begin (), impl->fonts.end (), xtl::bind (&remove_by_file_name, _1, file_name)), impl->fonts.end ());
}

void FontLibrary::UnloadFonts (const char* wildcard)
{
  if (!wildcard)
    return;

  impl->fonts.erase (stl::remove_if (impl->fonts.begin (), impl->fonts.end (), xtl::bind (&remove_by_wildcard, _1, wildcard)), impl->fonts.end ());
}

/*
   Создание шрифта
*/

Font FontLibrary::CreateFont (const char* name, const FontCreationParams& params)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    FontDesc* desc = Find (name);

    if (!desc)
      throw xtl::make_argument_exception ("", "name", name);

    return desc->CreateFont (params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::FontLibrary::CreateFont");
    throw;
  }
}

bool FontLibrary::CanCreateFont (const char* name, const FontCreationParams& params) const
{
  try
  {
    if (!name)
      return false;

    FontDesc* desc = Find (name);

    return desc && desc->CanCreateFont (params);
  }
  catch (...)
  {
    return false;
  }
}

/*
   Управление параметрами кэширования
*/

void FontLibrary::SetCacheState (bool state)
{
  impl->cache_state = state;
}

bool FontLibrary::CacheState () const
{
  return impl->cache_state;
}

void FontLibrary::SetCacheDir (const char* dir_name)
{
  if (!dir_name)
    throw xtl::make_null_argument_exception ("media::FontLibrary::SetCacheDir", "dir_name");

  impl->cache_dir = dir_name;
}

const char* FontLibrary::CacheDir () const
{
  return impl->cache_dir.c_str ();
}

/*
   Обмен
*/

void FontLibrary::Swap (FontLibrary& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (FontLibrary& library1, FontLibrary& library2)
{
  library1.Swap (library2);
}

}
