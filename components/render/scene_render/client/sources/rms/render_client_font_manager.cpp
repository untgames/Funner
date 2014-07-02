#include "shared.h"

using namespace render::scene::client;

/*
    Константы
*/

namespace
{

const char*  DEFAULT_SEMANTIC            = "font_mask"; //имя семантики по умолчанию
const char*  FONT_RESOURCE_PREFIX        = "font:";     //префикс имени шрифта
const char*  FONT_RESOURCE_WILDCARD      = "font:*";    //маска имени шрифта
const size_t FONT_RESOURCE_PREFIX_LENGTH = strlen (FONT_RESOURCE_PREFIX); //длина префикса шрифта

struct RasterizedFontCreationParamsImpl: public media::RasterizedFontCreationParams
{
  RasterizedFontCreationParamsImpl ()
  {
    max_image_size = 2048;
    pot            = true;
    glyph_margin   = 4;
    image_format   = media::PixelFormat_L8;
  }
};

RasterizedFontCreationParamsImpl rasterized_font_creation_params;

}

/*
    Описание реализации менеджера шрифтов
*/

namespace
{

/// Дескриптор шрифта
struct FontDesc
{
  Font*                font;       //шрифт
  xtl::auto_connection on_destroy; //соединение с обработчиком удаления шрифта

  FontDesc (Font* in_font) : font (in_font) {}
};

typedef stl::hash_map<size_t, FontDesc> FontMap;

/// Дескриптор материала шрифта
struct FontMaterialDesc
{
  FontMaterial*        font_material; //материал шрифта
  xtl::auto_connection on_destroy;    //соединение с обработчиком удаления материала шрифта

  FontMaterialDesc (FontMaterial* in_font_material)
    : font_material (in_font_material)
  {
  }
};

typedef stl::hash_map<size_t, FontMaterialDesc> FontMaterialMap;
typedef stl::list<media::FontLibrary>           FontLibraryList;

/// Кэш материалов шрифтов
struct FontMaterialCacheEntry
{
  media::Font     font;          //базовый шрифт
  FontMaterialPtr font_material; //материал шрифта

  FontMaterialCacheEntry (const media::Font& in_font, const FontMaterialPtr& in_font_material)
    : font (in_font)
    , font_material (in_font_material)
  {
  }
};

typedef stl::hash_map<stl::hash_key<const char*>, FontMaterialCacheEntry> FontMaterialCache;

}

struct FontManager::Impl
{
  MaterialManager&          material_manager;     //менеджер материалов
  FontMap                   fonts;                //шрифты
  FontMaterialMap           font_materials;       //материалы шрифтов
  FontLibraryList           font_libraries;       //библиотеки шрифтов
  FontMaterialCache         font_material_cache;  //кэш шрифтов
  FontRenderingTempCachePtr font_rendering_cache; //кэш рендеринга шрифтов

  Impl (MaterialManager& in_material_manager) : material_manager (in_material_manager) {}

  void RemoveFont (size_t id)
  {
    fonts.erase (id);
  }

  void RemoveFontMaterial (size_t hash)
  {
    font_materials.erase (hash);
  }
};

/*
    Конструктор / деструктор
*/

FontManager::FontManager (MaterialManager& material_manager)
  : impl (new Impl (material_manager))
{
}

FontManager::~FontManager ()
{
}

/*
    Создание шрифта / материала шрифта
*/

FontPtr FontManager::CreateFont (const media::Font& font)
{
  try
  {
    size_t id = font.Id ();

    FontMap::iterator iter = impl->fonts.find (id);

    if (iter != impl->fonts.end ())
      return iter->second.font;

    FontPtr new_font (new Font (impl->material_manager, font, rasterized_font_creation_params), false);

    xtl::auto_connection on_destroy = new_font->connect_tracker (xtl::bind (&Impl::RemoveFont, &*impl, id));

    iter = impl->fonts.insert_pair (id, &*new_font).first;

    iter->second.on_destroy.swap (on_destroy);

    return new_font;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::CreateFont");
    throw;
  }
}

FontMaterialPtr FontManager::CreateFontMaterial (const media::Font& font, const char* material, const char* semantic)
{
  try
  {
    if (!material)
      throw xtl::make_null_argument_exception ("", "material");

    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!*semantic)
      semantic = DEFAULT_SEMANTIC;

      //поиск материала

    static char SEPARATOR = '#';

    size_t id   = font.Id (),
           hash = common::crc32 (&id, sizeof (id), common::strhash (material, common::crc32 (&SEPARATOR, sizeof (SEPARATOR), common::strhash (semantic))));

    FontMaterialMap::iterator iter = impl->font_materials.find (hash);

    if (iter != impl->font_materials.end ())
      return iter->second.font_material;

      //создание шрифта

    FontPtr new_font = CreateFont (font);

      //создание нового материала

    FontMaterialPtr new_font_material (new FontMaterial (impl->material_manager, new_font, material, semantic), false);

    xtl::auto_connection on_destroy = new_font_material->connect_tracker (xtl::bind (&Impl::RemoveFontMaterial, &*impl, hash));

    iter = impl->font_materials.insert_pair (hash, &*new_font_material).first;

    iter->second.on_destroy.swap (on_destroy);

    return new_font_material;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::CreateFontMaterial");
    throw;
  }
}

/*
    Присоединение библиотеки шрифтов
*/

void FontManager::AttachFontLibrary (const media::FontLibrary& library)
{
  try
  {
    size_t id = library.Id ();

    for (FontLibraryList::iterator iter=impl->font_libraries.begin (), end=impl->font_libraries.end (); iter!=end; ++iter)
      if (id == iter->Id ())
        throw xtl::format_operation_exception ("", "Font library '%s' (with id %u) has been already defined", library.Name (), library.Id ());

    impl->font_libraries.push_back (library);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::AttachFontLibrary");
    throw;
  }
}

void FontManager::DetachFontLibrary (const media::FontLibrary& library)
{
  size_t id = library.Id ();

  for (FontLibraryList::iterator iter=impl->font_libraries.begin (), end=impl->font_libraries.end (); iter!=end; ++iter)
    if (id == iter->Id ())
    {
      impl->font_libraries.erase (iter);
      break;
    }
}

void FontManager::DetachAllFontLibraries ()
{
  impl->font_libraries.clear ();
}

/*
    Предварительный рендеринг шрифтов
*/

void FontManager::LoadFont (const char* id, const char* name, const media::FontCreationParams& params, const char* material, const char* semantic)
{
  try
  {
    if (!id)
      throw xtl::format_operation_exception ("", "id");

    if (!name)
      throw xtl::format_operation_exception ("", "name");

      //проверка повторной регистрации

    FontMaterialCache::iterator iter = impl->font_material_cache.find (id);

    if (iter != impl->font_material_cache.end ())
      throw xtl::make_argument_exception ("", "Font '%s' has been already loaded", id);

      //создание шрифта

    for (FontLibraryList::iterator iter=impl->font_libraries.begin (), end=impl->font_libraries.end (); iter!=end; ++iter)
    {
      media::FontLibrary& library = *iter;

      if (!library.CanCreateFont (name, params))
        continue;

      media::Font font = library.CreateFont (name, params);

        //создание материала шрифтов

      FontMaterialPtr font_material = CreateFontMaterial (font, material, semantic);

        //регистрация записи в кэше

      impl->font_material_cache.insert_pair (id, FontMaterialCacheEntry (font, font_material));

      return;
    }

    throw xtl::format_operation_exception ("", "Can't create font with id '%s'. None of attached font libraries can do this", id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::LoadFont(const char*,const char*,const media::FontCreationParams&,const char*, const char*)");
    throw;
  }
}

namespace
{

inline const char* get_string (const common::PropertyMap& properties, const char* name)
{
  return properties.GetString (name);
}

inline const char* get_string (const common::PropertyMap& properties, const char* name, const char* default_value)
{
  int index = properties.IndexOf (name);

  if (index == -1)
    return default_value;

  return properties.GetString ((size_t)index);
}

inline int get_int (const common::PropertyMap& properties, const char* name)
{
  return properties.GetInteger (name);
}

inline int get_int (const common::PropertyMap& properties, const char* name, int default_value)
{
  int index = properties.IndexOf (name);

  if (index == -1)
    return default_value;

  return properties.GetInteger ((size_t)index);
}

}

void FontManager::LoadFont (const char* init_string)
{
  try
  {
    if (!init_string)
      throw xtl::format_operation_exception ("", "init_string");

    if (!strncmp (FONT_RESOURCE_PREFIX, init_string, FONT_RESOURCE_PREFIX_LENGTH))
      init_string += FONT_RESOURCE_PREFIX_LENGTH;

      //разбор строки инициализации

    common::PropertyMap properties = common::parse_init_string (init_string);

    const char* font_name = get_string (properties, "font_name");
    const char* material  = get_string (properties, "material");
    const char* semantic  = get_string (properties, "semantic", "");

    media::FontCreationParams creation_params;

    memset (&creation_params, 0, sizeof (creation_params));

    creation_params.font_size     = (size_t)get_int (properties, "font_size");
    creation_params.font_size_eps = (size_t)get_int (properties, "font_size_eps", 0);
    creation_params.weight        = (size_t)get_int (properties, "weight", 0);
    creation_params.escapement    = (size_t)get_int (properties, "escapement", 0);
    creation_params.bold          = get_int (properties, "bold", 0) != 0;
    creation_params.italic        = get_int (properties, "italic", 0) != 0;
    creation_params.underlined    = get_int (properties, "underlined", 0) != 0;
    creation_params.striked       = get_int (properties, "striked", 0) != 0;
    creation_params.stroke_size   = (size_t)get_int (properties, "stroke_size", 0);
    creation_params.charset_name  = get_string (properties, "charset_name", "");

      //TODO: horizontal & vertical DPI support
    
      //загрузка шрифта

    LoadFont (init_string, font_name, creation_params, material, semantic);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontManager::LoadFont(const char* init_string = '%s')", init_string ? init_string : "");
    throw;
  }
}

void FontManager::UnloadFont (const char* id)
{
  if (!id)
    return;

  if (!strncmp (FONT_RESOURCE_PREFIX, id, FONT_RESOURCE_PREFIX_LENGTH))
    id += FONT_RESOURCE_PREFIX_LENGTH;

  impl->font_material_cache.erase (id);
}

/*
    Проверка соответствия имени ресурса параметрам настройки кэша шрифтов
*/

bool FontManager::IsFontParams (const char* resource)
{
  return resource && common::wcmatch (FONT_RESOURCE_WILDCARD, resource);
}

/*
    Кэш рендеринга шрифтов
*/

void FontManager::SetFontRenderingTempCache (const FontRenderingTempCachePtr& cache)
{
  impl->font_rendering_cache = cache;
}

const FontRenderingTempCachePtr& FontManager::FontRenderingTempCache () const
{
  return impl->font_rendering_cache;
}
