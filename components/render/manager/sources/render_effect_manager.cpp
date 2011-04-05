#include "shared.h"

using namespace render;

namespace
{

typedef stl::vector<EffectProxy>  EffectProxyList;
typedef stl::vector<SamplerProxy> SamplerProxyList;
typedef stl::vector<ProgramProxy> ProgramProxyList;

struct EffectLibraryEntry: public xtl::reference_counter
{
  stl::string      resource_name;  //имя ресурса
  EffectProxyList  effects;        //эффекты библиотеки
  SamplerProxyList samplers;       //сэмплеры библиотеки
  ProgramProxyList programs;       //программы библиотеки

  EffectLibraryEntry () {}

  ~EffectLibraryEntry ()
  {
    Clear (effects);
    Clear (samplers);
    Clear (programs);
  }
  
  void AddEffect (const char* id, const EffectPtr& effect, EffectProxyManager& proxy_manager)
  {
    EffectProxy proxy = proxy_manager.GetProxy (id);

    proxy.SetResource (effect);
      
    effects.push_back (proxy);
  }
  
  void AddProgram (const char* id, const LowLevelProgramPtr& program, const ShadingManagerPtr& shading_manager)
  {
    ProgramProxy proxy = shading_manager->GetProgramProxy (id);
    
    proxy.SetResource (program);
    
    programs.push_back (proxy);
  }
  
  void AddSampler (const char* id, const LowLevelSamplerStatePtr& sampler, const TextureManagerPtr& texture_manager)
  {
    SamplerProxy proxy = texture_manager->GetSamplerProxy (id);
    
    proxy.SetResource (sampler);
    
    samplers.push_back (proxy);
  }
  
  template <class T> void Clear (stl::vector<ResourceProxy<T> >& items)
  {
    while (!items.empty ())
    {
      items.back ().SetResource (T ());
      items.pop_back ();
    }
  }
};

typedef xtl::intrusive_ptr<EffectLibraryEntry> EffectLibraryEntryPtr;
typedef stl::list<EffectLibraryEntryPtr>       EffectLibraryList;

}

/*
    Описание реализации менеджера эффектов
*/

struct EffectManager::Impl
{
  DeviceManagerPtr    device_manager;   //менеджер устройства отрисовки
  TextureManagerPtr   texture_manager;  //менеджер текстур
  ShadingManagerPtr   shading_manager;  //менеджер шейдинга
  EffectProxyManager  proxy_manager;    //менеджер прокси эффектов
  EffectLibraryList   loaded_libraries; //список загруженных библиотек

///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ShadingManagerPtr& in_shading_manager)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , shading_manager (in_shading_manager)
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!texture_manager)
      throw xtl::make_null_argument_exception ("", "texture_manager");
      
    if (!shading_manager)
      throw xtl::make_null_argument_exception ("", "shading_manager");
  }
};

/*
    Конструктор / деструктор
*/

EffectManager::EffectManager (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ShadingManagerPtr& shading_manager)
{
  try
  {
    impl = new Impl (device_manager, texture_manager, shading_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::EffectManager::EffectManager");
    throw;
  }
}

EffectManager::~EffectManager ()
{
}

/*
    Проверка: является ли ресурс библиотекой эффектов
*/

bool EffectManager::IsEffectLibraryResource (const char* name)
{
  return is_effect_library_file (name);
}

/*
    Загрузка / выгрузка эффектов
*/

void EffectManager::LoadEffectLibrary (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
      //проверка повторной загрузки
    
    for (EffectLibraryList::iterator iter=impl->loaded_libraries.begin (), end=impl->loaded_libraries.end (); iter!=end; ++iter)
    {
      EffectLibraryEntry& entry = **iter;
      
      if (entry.resource_name == name)
        throw xtl::format_operation_exception ("", "Effect library '%s' has been alredy loaded", name);
    }

      //создание новой библиотеки

    EffectLoaderLibrary library;

    parse_effect_library (impl->device_manager, name, library);

    EffectLibraryEntryPtr entry (new EffectLibraryEntry, false);

    entry->resource_name = name;

    library.Effects ().ForEach       (xtl::bind (&EffectLibraryEntry::AddEffect, &*entry, _1, _2, xtl::ref (impl->proxy_manager)));
    library.Programs ().ForEach      (xtl::bind (&EffectLibraryEntry::AddProgram, &*entry, _1, _2, xtl::ref (impl->shading_manager)));
    library.SamplerStates ().ForEach (xtl::bind (&EffectLibraryEntry::AddSampler, &*entry, _1, _2, xtl::ref (impl->texture_manager)));

      //регистрация библиотеки
      
    impl->loaded_libraries.push_back (entry);          
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::EffectManager::LoadEffectLibrary");
    throw;
  }
}

void EffectManager::UnloadEffectLibrary (const char* name)
{
  if (!name)
    return;      

  for (EffectLibraryList::iterator iter=impl->loaded_libraries.begin (), end=impl->loaded_libraries.end (); iter!=end; ++iter)
  {
    EffectLibraryEntry& entry = **iter;
    
    if (entry.resource_name == name)
    {
      impl->loaded_libraries.erase (iter);
      return;
    }
  }
}

/*
    Получение прокси
*/

EffectProxy EffectManager::GetEffectProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    Поиск загруженного эффекта
*/

EffectPtr EffectManager::FindEffect (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    Установка эффекта по умолчанию
*/

void EffectManager::SetDefaultEffect (const EffectPtr& effect)
{
  impl->proxy_manager.SetDefaultResource (effect);
}

EffectPtr EffectManager::DefaultEffect ()
{
  return impl->proxy_manager.DefaultResource ();
}
