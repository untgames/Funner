#include "shared.h"

using namespace render::manager;

namespace
{

//TODO: ошибка при наложении имен ресурсов: выгрузка библиотеки не проверяет является ли она текущим владельцем ресурса

/*
    Константы
*/

const char* RFX_FILE_SUFFIX = ".rfx"; //суффикс имени файла описания библиотеки эффектов

/*
    Описание библиотеки эффектов
*/

typedef stl::vector<EffectProxy>      EffectProxyList;
typedef stl::vector<SamplerProxy>     SamplerProxyList;
typedef stl::vector<ProgramProxy>     ProgramProxyList;
typedef stl::vector<TextureDescProxy> TextureDescProxyList;

struct EffectLibraryEntry: public xtl::reference_counter
{
  stl::string          resource_name;      //имя ресурса
  common::ParseNode    root_node;          //корневой узел описания библиотеки эффектов
  common::ParseNode    resolved_root_node; //корневой узел описания библиотеки эффектов после разрешения ссылок
  EffectProxyList      effects;            //эффекты библиотеки
  SamplerProxyList     samplers;           //сэмплеры библиотеки
  TextureDescProxyList texture_descs;      //описатели текстур
  ProgramProxyList     programs;           //программы библиотеки

///Конструктор
  EffectLibraryEntry () {}

///Деструктор
  ~EffectLibraryEntry ()
  {
    Clear (effects);
    Clear (samplers);
    Clear (texture_descs);
    Clear (programs);
  }
  
///Добавление эффекта
  void AddEffect (const char* id, const EffectPtr& effect, EffectProxyManager& proxy_manager)
  {
    EffectProxy proxy = proxy_manager.GetProxy (id);

    proxy.SetResource (effect);
      
    effects.push_back (proxy);
  }
  
///Добавление программы
  void AddProgram (const char* id, const ProgramPtr& program, const ProgramManagerPtr& program_manager)
  {
    ProgramProxy proxy = program_manager->GetProgramProxy (id);
    
    proxy.SetResource (program);
    
    programs.push_back (proxy);
  }
  
///Добавление сэмплера
  void AddSampler (const char* id, const LowLevelSamplerStatePtr& sampler, const TextureManagerPtr& texture_manager)
  {
    SamplerProxy proxy = texture_manager->GetSamplerProxy (id);
    
    proxy.SetResource (sampler);
    
    samplers.push_back (proxy);
  }

///Добавление описателя текстуры
  void AddTextureDesc (const char* id, const LowLevelTextureDescPtr& texture_desc, const TextureManagerPtr& texture_manager)
  {
    TextureDescProxy proxy = texture_manager->GetTextureDescProxy (id);
    
    proxy.SetResource (texture_desc);
    
    texture_descs.push_back (proxy);
  }
    
///Очистка коллекции
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
  DeviceManagerPtr        device_manager;            //менеджер устройства отрисовки
  TextureManagerPtr       texture_manager;           //менеджер текстур
  ProgramManagerPtr       program_manager;           //менеджер программ
  EffectProxyManager      proxy_manager;             //менеджер прокси эффектов
  EffectLibraryList       loaded_libraries;          //список загруженных библиотек
  common::PropertyMap     settings;                  //настройки
  common::ParseNode       configuration;             //конфигурация рендеринга
  bool                    need_update_configuration; //конфигураци рендеринга должна быть обновлена
  IEffectManagerListener* listener;                  //слушатель событий менеджера эффектов  

///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ProgramManagerPtr& in_program_manager, IEffectManagerListener* in_listener)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , program_manager (in_program_manager)
    , need_update_configuration (false)
    , listener (in_listener)
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!texture_manager)
      throw xtl::make_null_argument_exception ("", "texture_manager");
      
    if (!program_manager)
      throw xtl::make_null_argument_exception ("", "program_manager");
  }
  
///Оповещение о необходимости обновления конфигурации
  void UpdateConfigurationNotify ()
  {
    if (need_update_configuration)
      return;
      
    need_update_configuration = true;
      
    if (listener)
      listener->OnConfigurationChanged ();    
  }
  
///Обновление библиотеки ресурсов
  void UpdateLibrary (const EffectLibraryEntryPtr& entry)
  {
    EffectLoaderLibrary library;
    
    parse_effect_library (device_manager, entry->resolved_root_node, entry->resource_name.c_str (), library);

    library.Effects ().ForEach       (xtl::bind (&EffectLibraryEntry::AddEffect, &*entry, _1, _2, xtl::ref (proxy_manager)));
    library.Programs ().ForEach      (xtl::bind (&EffectLibraryEntry::AddProgram, &*entry, _1, _2, xtl::ref (program_manager)));
    library.SamplerStates ().ForEach (xtl::bind (&EffectLibraryEntry::AddSampler, &*entry, _1, _2, xtl::ref (texture_manager)));
    library.TextureDescs ().ForEach  (xtl::bind (&EffectLibraryEntry::AddTextureDesc, &*entry, _1, _2, xtl::ref (texture_manager)));    
  }  
};

/*
    Конструктор / деструктор
*/

EffectManager::EffectManager (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ProgramManagerPtr& program_manager, IEffectManagerListener* listener)
{
  try
  {
    impl = new Impl (device_manager, texture_manager, program_manager, listener);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectManager::EffectManager");
    throw;
  }
}

EffectManager::~EffectManager ()
{
}

/*
    Проверка: является ли ресурс библиотекой эффектов
*/

bool EffectManager::IsEffectLibraryResource (const char* file_name)
{
  if (!file_name)
    return false;

  return xtl::xstricmp (common::suffix (file_name).c_str (), RFX_FILE_SUFFIX) == 0;
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

    common::Parser parser (name, "wxf");

    EffectLibraryEntryPtr entry (new EffectLibraryEntry, false);

    entry->resource_name      = name;
    entry->root_node          = parser.Root ();
    entry->resolved_root_node = resolve_references (entry->root_node, impl->settings);
    
    impl->UpdateLibrary (entry);

      //регистрация библиотеки
      
    impl->loaded_libraries.push_back (entry);
    
      //обновление конфигурации
      
    impl->UpdateConfigurationNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectManager::LoadEffectLibrary");
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

      impl->UpdateConfigurationNotify ();      

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

/*
    Применение настроек / получение конфигурации ренедринга эффектов
*/

void EffectManager::ChangeSettings (const common::PropertyMap& settings)
{
  try
  {
      //разрешение ссылок
    
    stl::vector<common::ParseNode> resolved_nodes;

    resolved_nodes.reserve (impl->loaded_libraries.size ());

    for (EffectLibraryList::iterator iter=impl->loaded_libraries.begin (), end=impl->loaded_libraries.end (); iter!=end; ++iter)
      resolved_nodes.push_back (resolve_references ((*iter)->root_node, settings));
      
    size_t index = 0;
      
    for (EffectLibraryList::iterator iter=impl->loaded_libraries.begin (), end=impl->loaded_libraries.end (); iter!=end; ++iter, ++index)
      (*iter)->resolved_root_node = resolved_nodes [index];
      
      //перезагрузка библиотеки
      
    for (EffectLibraryList::iterator iter=impl->loaded_libraries.begin (), end=impl->loaded_libraries.end (); iter!=end; ++iter)
    {
      EffectLibraryEntryPtr entry (new EffectLibraryEntry, false);
      
      entry->resource_name      = (*iter)->resource_name;
      entry->root_node          = (*iter)->root_node;
      entry->resolved_root_node = (*iter)->resolved_root_node;

      impl->UpdateLibrary (entry);
      
      *iter = entry;
    }

      //замена настроек

    impl->settings = settings;
    
      //оповещеие об изменении настроек
      
    impl->UpdateConfigurationNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectManager::ChangeSettings");
    throw;
  }
}

common::ParseNode EffectManager::Configuration ()
{
  try
  {
    if (!impl->need_update_configuration)    
      return impl->configuration;

    stl::vector<common::ParseNode> nodes;

    nodes.reserve (impl->loaded_libraries.size ());

    for (EffectLibraryList::iterator iter=impl->loaded_libraries.begin (), end=impl->loaded_libraries.end (); iter!=end; ++iter)
      nodes.push_back ((*iter)->resolved_root_node);

    impl->configuration = concat (nodes.size (), &nodes [0]);

    return impl->configuration;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::EffectManager::Configuration");
    throw;
  }
}
