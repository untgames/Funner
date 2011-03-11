#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера материалов
*/

namespace
{

typedef stl::vector<MaterialProxy> MaterialProxyList;

struct MaterialLibraryEntry: public xtl::reference_counter
{
  const media::rfx::MaterialLibrary* source_library; //ссылка на исходную библиотеку материалов (может быть недействительна на момент обращения)
  stl::string                        resource_name;  //имя ресурса
  MaterialProxyList                  materials;      //материалы библиотеки
  
  MaterialLibraryEntry () : source_library (0) {}
  
  ~MaterialLibraryEntry ()
  {
    while (!materials.empty ())
    {
      materials.back ().SetResource (MaterialPtr ());
      materials.pop_back ();
    }
  }
};

typedef xtl::intrusive_ptr<MaterialLibraryEntry> MaterialLibraryEntryPtr;
typedef stl::list<MaterialLibraryEntryPtr>       MaterialLibraryList;

}

struct MaterialManager::Impl
{
  DeviceManagerPtr      device_manager;   //менеджер устройства отрисовки
  TextureManagerPtr     texture_manager;  //менеджер текстур
  MaterialProxyManager  proxy_manager;    //менеджер прокси объектов
  MaterialLibraryList   loaded_libraries; //список загруженных библиотек
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
  {
  }
  
///Загрузка библиотеки
  void LoadMaterialLibrary (const media::rfx::MaterialLibrary& library, const char* name)
  {        
    try
    {
        //проверка повторной загрузки
      
      for (MaterialLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
      {
        MaterialLibraryEntry& entry = **iter;
        
        if (entry.source_library == &library || entry.resource_name == name)
          throw xtl::format_operation_exception ("", "Material library '%s' has been alredy loaded", *name ? name : *library.Name () ? library.Name () : "<reference>");
      }
      
        //создание новой библиотеки
        
      MaterialLibraryEntryPtr entry (new MaterialLibraryEntry, false);
      
      entry->source_library = &library;
      entry->resource_name  = name;
      
      entry->materials.reserve (library.Size ());
      
      for (media::rfx::MaterialLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
      {
        MaterialPtr material (new MaterialImpl (texture_manager), false);
        
        material->Update (*iter);
        
        MaterialProxy proxy = proxy_manager.GetProxy (library.ItemId (iter));
        
        proxy.SetResource (material);
        
        entry->materials.push_back (proxy);
      }
      
        //регистрация библиотеки
        
      loaded_libraries.push_back (entry);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::MaterialManager::LoadMaterialLibrary");
      throw;
    }
  }
  
///Выгрузка библиотеки
  void UnloadMaterialLibrary (const media::rfx::MaterialLibrary* source_library, const char* name)
  {
    if (!name)
      name = "";
    
    for (MaterialLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
    {
      MaterialLibraryEntry& entry = **iter;
      
      if (entry.source_library == source_library || entry.resource_name == name)
      {
        loaded_libraries.erase (iter);
        return;
      }
    }    
  }
};

/*
    Конструктор / деструктор
*/

MaterialManager::MaterialManager (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager)
  : impl (new Impl (device_manager, texture_manager))
{
}

MaterialManager::~MaterialManager ()
{
}

/*
    Создание материалов
*/

MaterialPtr MaterialManager::CreateMaterial ()
{
  try
  {
    return MaterialPtr (new MaterialImpl (impl->texture_manager), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::CreateMaterial()");
    throw;
  }
}

/*
    Проверка: является ли ресурс библиотекой материалов
*/

bool MaterialManager::IsMaterialLibraryResource (const char* name)
{
  return name && media::rfx::MaterialLibraryManager::FindLoader (name, common::SerializerFindMode_ByName) != 0;
}

/*
    Загрузка / выгрузка библиотек материалов
*/

void MaterialManager::LoadMaterialLibrary (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    media::rfx::MaterialLibrary library (name);

    impl->LoadMaterialLibrary (library, name);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::LoadMaterialLibrary(const char*)");
    throw;
  }
}

void MaterialManager::LoadMaterialLibrary (const media::rfx::MaterialLibrary& library)
{
  try
  {
    impl->LoadMaterialLibrary (library, ""); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::LoadMaterialLibrary(const media::rfx::MaterialLibrary&)");
    throw;
  }
}

void MaterialManager::UnloadMaterialLibrary (const char* name)
{
  if (!name)
    return;
    
  impl->UnloadMaterialLibrary (0, name);
}

void MaterialManager::UnloadMaterialLibrary (const media::rfx::MaterialLibrary& library)
{
  impl->UnloadMaterialLibrary (&library, 0);
}

/*
    Получение прокси
*/

MaterialProxy MaterialManager::GetMaterialProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    Поиск загруженного материала
*/

MaterialPtr MaterialManager::FindMaterial (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    Установка материала по умолчанию
*/

void MaterialManager::SetDefaultMaterial (const MaterialPtr& material)
{
  impl->proxy_manager.SetDefaultResource (material);
}

MaterialPtr MaterialManager::DefaultMaterial ()
{
  return impl->proxy_manager.DefaultResource ();
}
