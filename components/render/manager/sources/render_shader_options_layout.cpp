#include "shared.h"

using namespace render::manager;

namespace
{

/*
    Вспомогательный класс построения опций шейдера
*/

struct ShaderOptionsBuilder: public xtl::reference_counter
{
  struct OptionDesc
  {
    const char*          name;  //имя свойства
    int                  index; //индекс свойства в карте свойств
    common::PropertyType type;  //тип свойства
  };
  
  typedef stl::vector<OptionDesc> OptionDescArray;
  
  common::StringArray defines;      //строки с именами опций
  OptionDescArray     option_descs; //список опций
  
///Конструктор
  ShaderOptionsBuilder (const common::PropertyLayout& layout, const common::StringArray& in_defines)
    : defines (in_defines.Clone ())
  {
    option_descs.reserve (defines.Size ());
    
    const char** define_names = defines.Data ();

    for (size_t i=0, count=defines.Size (); i<count; i++)
    {
      OptionDesc option = {define_names [i], layout.IndexOf (define_names [i])};
      
      if (option.index == -1)
        continue;

      option.type = layout.PropertyType (option.index);
      
      option_descs.push_back (option);
    }
  }
  
///Получение опций компиляции шейдера
  void GetShaderOptions (const common::PropertyMap& properties, ShaderOptions& out_options, stl::string& value_buffer)
  {
    out_options.options.clear ();

    bool need_add_space = false;
    
    for (OptionDescArray::iterator iter=option_descs.begin (), end=option_descs.end (); iter!=end; ++iter)
    {
      const OptionDesc& option_desc = *iter;

      if (option_desc.index == -1)
        continue;
      
      if (need_add_space)
        out_options.options += ' ';

        //TODO: сделать формирование массивов свойств (в зависимости от типа)

      need_add_space       = true;
      out_options.options += option_desc.name;     
      
      properties.GetProperty (option_desc.index, value_buffer);
      
      out_options.options += '=';
      out_options.options += value_buffer;
    }
    
    out_options.options_hash = common::strhash (out_options.options.c_str ());
  }
};

typedef xtl::intrusive_ptr<ShaderOptionsBuilder> ShaderOptionsBuilderPtr;

/*
    Ключ поиска объекта построения опций шейдера
*/

struct ShaderOptionsBuilderKey
{
  size_t layout_hash;      //хэш лэйаута
  size_t properties_count; //количество свойств
  
  ShaderOptionsBuilderKey (size_t in_layout_hash, size_t in_properties_count)
    : layout_hash (in_layout_hash)
    , properties_count (in_properties_count)
  {
  }
  
  bool operator == (const ShaderOptionsBuilderKey& key) const
  {
    return layout_hash == key.layout_hash && properties_count == key.properties_count;
  }
};

size_t hash (const ShaderOptionsBuilderKey& key)
{
  return key.layout_hash * key.properties_count;
}

}

/*
    Описание реализации списка макро-определений шейдера
*/

typedef CacheMap<ShaderOptionsBuilderKey, ShaderOptionsBuilderPtr> ShaderOptionsBuilderMap;

struct ShaderOptionsLayout::Impl: public xtl::trackable
{
  common::StringArray     defines;      //список названий макро-определений
  ShaderOptionsBuilderMap builders;     //генераторы опций шейдера
  stl::string             value_buffer; //временный буфер для формирования опций
  bool                    need_flush;   //кэш требуется сбросить
  size_t                  hash;         //хэш лэйаута
  
///Конструктор
  Impl (const CacheManagerPtr& cache_manager)
    : builders (cache_manager)
    , need_flush (false)
    , hash (0xffffffff)
  {
  }
  
///Сброс кэша
  void FlushCache ()
  {
    builders.Clear ();

    hash = 0xffffffff;

    const char** define_names = defines.Data ();

    for (size_t i=0, count=defines.Size (); i<count; i++)
    {
      if (i)
        hash = common::strhash (" ", hash);
      
      hash = common::strhash (define_names [i], hash);
    }
    
    need_flush = false;
  }
};

/*
    Конструктор / деструктор
*/

ShaderOptionsLayout::ShaderOptionsLayout (const CacheManagerPtr& cache_manager)
{
  try
  {
    impl = new Impl (cache_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::ShaderOptionsLayout::ShaderOptionsLayout");
    throw;
  }
}

ShaderOptionsLayout::~ShaderOptionsLayout ()
{
}

/*
    Количество закэшированных генераторов опций
*/

size_t ShaderOptionsLayout::CachedShaderBuildersCount ()
{
  if (impl->need_flush)
    impl->FlushCache ();

  return impl->builders.Size ();
}

/*
    Макро-определения
*/

size_t ShaderOptionsLayout::Size ()
{
  return impl->defines.Size ();
}

const char* ShaderOptionsLayout::Item (size_t index)
{
  return impl->defines [index];
}

void ShaderOptionsLayout::Add (const char* name)
{
  impl->defines    += name;
  impl->need_flush  = true;
}

void ShaderOptionsLayout::Remove (const char* name)
{
  if (!name)
    return;

  const char** defines = impl->defines.Data ();

  for (size_t i=0, count=impl->defines.Size (); i<count; i++)
    if (!strcmp (defines [i], name))
    {
      impl->defines.Remove (i);
      
      i--;
      count--;
      
      impl->need_flush = true;
    }
}

void ShaderOptionsLayout::Clear ()
{
  impl->defines.Clear ();

  impl->need_flush = true;
}

/*
    Получение опций шейдера
*/

void ShaderOptionsLayout::GetShaderOptions (const common::PropertyMap& defines, ShaderOptions& out_options)
{
  try
  {
    if (impl->need_flush)
      impl->FlushCache ();
      
    ShaderOptionsBuilderKey key (defines.Layout ().Hash (), defines.Layout ().Size ());
    
    if (ShaderOptionsBuilderPtr* builder = impl->builders.Find (key))
    {
      (*builder)->GetShaderOptions (defines, out_options, impl->value_buffer);
      return;
    }
    
    ShaderOptionsBuilderPtr builder (new ShaderOptionsBuilder (defines.Layout (), impl->defines), false);
    
    impl->builders.Add (key, builder);
    
    builder->GetShaderOptions (defines, out_options, impl->value_buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::ShaderOptionsLayout::GetShaderOptions");
    throw;
  }
}

/*
    Хэш
*/

size_t ShaderOptionsLayout::Hash ()
{
  try
  {
    if (impl->need_flush)
      impl->FlushCache ();
    
    return impl->hash;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::ShaderOptionsLayout::Hash");
    throw;
  }
}

/*
    Получение объекта оповещения об удалении
*/

xtl::trackable& ShaderOptionsLayout::GetTrackable ()
{
  return *impl;
}
