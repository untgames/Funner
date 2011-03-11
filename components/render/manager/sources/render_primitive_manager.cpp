#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера примитивов
*/

namespace
{

typedef stl::vector<PrimitiveProxy> PrimitiveProxyList;

struct MeshLibraryEntry: public xtl::reference_counter
{
  const media::geometry::MeshLibrary* source_library; //ссылка на исходную библиотеку мешей (может быть недействительна на момент обращения)
  stl::string                         resource_name;  //имя ресурса
  PrimitiveProxyList                  primitives;     //примтитивы библиотеки
  
  MeshLibraryEntry () : source_library (0) {}
  
  ~MeshLibraryEntry ()
  {
    while (!primitives.empty ())
    {
      primitives.back ().SetResource (PrimitivePtr ());
      primitives.pop_back ();
    }
  }
};

typedef xtl::intrusive_ptr<MeshLibraryEntry> MeshLibraryEntryPtr;
typedef stl::list<MeshLibraryEntryPtr>       MeshLibraryList;

}

struct PrimitiveManager::Impl
{
  DeviceManagerPtr      device_manager;   //менеджер устройства отрисовки
  PrimitiveProxyManager proxy_manager;    //менеджер прокси объектов
  MeshLibraryList       loaded_libraries; //список загруженных библиотек
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
  {
  }
  
///Загрузка библиотеки
  void LoadMeshLibrary (const media::geometry::MeshLibrary& library, const char* name)
  {        
    try
    {
        //проверка повторной загрузки
      
      for (MeshLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
      {
        MeshLibraryEntry& entry = **iter;
        
        if (entry.source_library == &library || entry.resource_name == name)
          throw xtl::format_operation_exception ("", "Mesh library '%s' has been alredy loaded", *name ? name : *library.Name () ? library.Name () : "<reference>");
      }
      
        //создание буферов примитивов библиотеки мэшей
        
      PrimitiveBuffersPtr primitive_buffers (new PrimitiveBuffersImpl (device_manager, MeshBufferUsage_Default, MeshBufferUsage_Default), false);
      
        //создание новой библиотеки
        
      MeshLibraryEntryPtr entry (new MeshLibraryEntry, false);
      
      entry->source_library = &library;
      entry->resource_name  = name;
      
      entry->primitives.reserve (library.Size ());
      
      primitive_buffers->EnableCache ();      
      
      for (media::geometry::MeshLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
      {
        PrimitivePtr primitive (new PrimitiveImpl (device_manager, primitive_buffers), false);
        
        PrimitiveProxy proxy = proxy_manager.GetProxy (library.ItemId (iter));
        
        proxy.SetResource (primitive);
        
        entry->primitives.push_back (proxy);
      }

      primitive_buffers->DisableCache ();
      
        //регистрация библиотеки
        
      loaded_libraries.push_back (entry);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::PrimitiveManager::LoadMeshLibrary");
      throw;
    }
  }
  
///Выгрузка библиотеки
  void UnloadMeshLibrary (const media::geometry::MeshLibrary* source_library, const char* name)
  {
    if (!name)
      name = "";
    
    for (MeshLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
    {
      MeshLibraryEntry& entry = **iter;
      
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

PrimitiveManager::PrimitiveManager (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

PrimitiveManager::~PrimitiveManager ()
{
}

/*
    Создание примитивов
*/

PrimitivePtr PrimitiveManager::CreatePrimitive ()
{
  try
  {
    return CreatePrimitive (CreatePrimitiveBuffers ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::CreatePrimitive()");
    throw;
  }
}

PrimitivePtr PrimitiveManager::CreatePrimitive (const PrimitiveBuffersPtr& buffer)
{
  try
  {
    return PrimitivePtr (new PrimitiveImpl (impl->device_manager, buffer), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::CreatePrimitive(const PrimitiveBuffersPtr&)");
    throw;
  }
}

PrimitiveBuffersPtr PrimitiveManager::CreatePrimitiveBuffers (MeshBufferUsage lines_usage, MeshBufferUsage sprites_usage)
{
  try
  {
    return PrimitiveBuffersPtr (new PrimitiveBuffersImpl (impl->device_manager, lines_usage, sprites_usage), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::CreatePrimitiveBuffers");
    throw;
  }
}

/*
    Проверка: является ли ресурс библиотекой мешей
*/

bool PrimitiveManager::IsMeshLibraryResource (const char* name)
{
  return name && media::geometry::MeshLibraryManager::FindLoader (name, common::SerializerFindMode_ByName) != 0;
}

/*
    Загрузка / выгрузка библиотек мешей
*/

void PrimitiveManager::LoadMeshLibrary (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    media::geometry::MeshLibrary library (name);

    impl->LoadMeshLibrary (library, name);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::LoadMeshLibrary(const char*)");
    throw;
  }
}

void PrimitiveManager::LoadMeshLibrary (const media::geometry::MeshLibrary& library)
{
  try
  {
    impl->LoadMeshLibrary (library, ""); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::LoadMeshLibrary(const media::geometry::MeshLibrary&)");
    throw;
  }
}

void PrimitiveManager::UnloadMeshLibrary (const char* name)
{
  if (!name)
    return;
    
  impl->UnloadMeshLibrary (0, name);
}

void PrimitiveManager::UnloadMeshLibrary (const media::geometry::MeshLibrary& library)
{
  impl->UnloadMeshLibrary (&library, 0);
}

/*
    Получение прокси
*/

PrimitiveProxy PrimitiveManager::GetPrimitiveProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    Поиск загруженного примитива
*/

PrimitivePtr PrimitiveManager::FindPrimitive (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    Установка примитива по умолчанию
*/

void PrimitiveManager::SetDefaultPrimitive (const PrimitivePtr& primitive)
{
  impl->proxy_manager.SetDefaultResource (primitive);
}

PrimitivePtr PrimitiveManager::DefaultPrimitive ()
{
  return impl->proxy_manager.DefaultResource ();
}
