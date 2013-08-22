#include "shared.h"

using namespace render::manager;

/*
    �������� ���������� ��������� ����������
*/

namespace
{

typedef stl::vector<PrimitiveProxy> PrimitiveProxyList;

struct MeshLibraryEntry: public xtl::reference_counter
{
  const media::geometry::MeshLibrary* source_library; //������ �� �������� ���������� ����� (����� ���� ��������������� �� ������ ���������)
  stl::string                         resource_name;  //��� �������
  PrimitiveProxyList                  primitives;     //��������� ����������
  
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

typedef xtl::intrusive_ptr<MeshLibraryEntry>                      MeshLibraryEntryPtr;
typedef stl::list<MeshLibraryEntryPtr>                            MeshLibraryList;
typedef stl::hash_map<stl::hash_key<const char*>, PrimitiveProxy> PrimitiveProxyMap;

}

struct PrimitiveManager::Impl
{
  DeviceManagerPtr      device_manager;    //�������� ���������� ���������
  MaterialManagerPtr    material_manager;  //�������� ����������
  PrimitiveProxyManager proxy_manager;     //�������� ������ ��������
  MeshLibraryList       loaded_libraries;  //������ ����������� ���������
  PrimitiveProxyMap     shared_primitives; //������ ��������� ������������ ����������
  Log                   log;               //����� ����������������
  
///�����������
  Impl (const DeviceManagerPtr& in_device_manager, const MaterialManagerPtr& in_material_manager)
    : device_manager (in_device_manager)
    , material_manager (in_material_manager)
  {
  }
  
///�������� ����������
  void LoadMeshLibrary (const media::geometry::MeshLibrary& library, const char* name)
  {        
    try
    {
        //�������� ��������� ��������
      
      for (MeshLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
      {
        MeshLibraryEntry& entry = **iter;
        
        if (entry.source_library == &library || entry.resource_name == name)
          throw xtl::format_operation_exception ("", "Mesh library '%s' has been alredy loaded", *name ? name : *library.Name () ? library.Name () : "<reference>");
      }
      
        //�������� ������� ���������� ���������� �����
        
      PrimitiveBuffersPtr primitive_buffers (new PrimitiveBuffersImpl (device_manager, MeshBufferUsage_Default, MeshBufferUsage_Default), false);
      
        //�������� ����� ����������
        
      MeshLibraryEntryPtr entry (new MeshLibraryEntry, false);
      
      entry->source_library = &library;
      entry->resource_name  = name;
      
      entry->primitives.reserve (library.Size ());
      
      primitive_buffers->EnableCache ();      
      
      for (media::geometry::MeshLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
      {
        PrimitivePtr primitive (new PrimitiveImpl (device_manager, material_manager, primitive_buffers, common::format ("%s(%s)", iter->Name (), library.Name ()).c_str ()), false);
        
        primitive->AddMesh (*iter, MeshBufferUsage_Default, MeshBufferUsage_Default);
        
        PrimitiveProxy proxy = proxy_manager.GetProxy (library.ItemId (iter));
        
        proxy.SetResource (primitive);
        
        entry->primitives.push_back (proxy);
      }

      primitive_buffers->DisableCache ();
      
        //����������� ����������
        
      loaded_libraries.push_back (entry);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::PrimitiveManager::LoadMeshLibrary");
      throw;
    }
  }
  
///�������� ����������
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
    ����������� / ����������
*/

PrimitiveManager::PrimitiveManager (const DeviceManagerPtr& device_manager, const MaterialManagerPtr& material_manager)
  : impl (new Impl (device_manager, material_manager))
{
}

PrimitiveManager::~PrimitiveManager ()
{
}

/*
    �������� ����������
*/

PrimitivePtr PrimitiveManager::CreatePrimitive ()
{
  try
  {
    return CreatePrimitive (CreatePrimitiveBuffers ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveManager::CreatePrimitive()");
    throw;
  }
}

PrimitivePtr PrimitiveManager::CreatePrimitive (const PrimitiveBuffersPtr& buffer)
{
  try
  {
    return PrimitivePtr (new PrimitiveImpl (impl->device_manager, impl->material_manager, buffer), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveManager::CreatePrimitive(const PrimitiveBuffersPtr&)");
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
    e.touch ("render::manager::PrimitiveManager::CreatePrimitiveBuffers");
    throw;
  }
}

/*
    ��������: �������� �� ������ ����������� �����
*/

bool PrimitiveManager::IsMeshLibraryResource (const char* name)
{
  return name && media::geometry::MeshLibraryManager::FindLoader (name, common::SerializerFindMode_ByName) != 0;
}

/*
    �������� / �������� ��������� �����
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
    e.touch ("render::manager::PrimitiveManager::LoadMeshLibrary(const char*)");
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
    e.touch ("render::manager::PrimitiveManager::LoadMeshLibrary(const media::geometry::MeshLibrary&)");
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
    ����������� ��������� ������������ ����������
*/

void PrimitiveManager::SharePrimitive (const char* name, const PrimitivePtr& primitive)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!primitive)
      throw xtl::make_null_argument_exception ("", "primitive");
      
    impl->log.Printf ("Share primitive '%s'", name);
      
    if (impl->shared_primitives.find (name) != impl->shared_primitives.end ())
      throw xtl::format_operation_exception ("", "Primitive '%s' has been already loaded", name);

    PrimitiveProxy proxy = impl->proxy_manager.GetProxy (name);

    proxy.SetResource (primitive);
    
    impl->shared_primitives.insert_pair (name, proxy);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveManager::SharePrimitive");
    throw;
  }
}

void PrimitiveManager::UnsharePrimitive (const char* name)
{
  if (!name)
    return;
    
  PrimitiveProxyMap::iterator iter = impl->shared_primitives.find (name);
  
  if (iter == impl->shared_primitives.end ())
    return;    
    
  iter->second.SetResource (PrimitivePtr ());
  iter->second.ResetCache ();
    
  impl->shared_primitives.erase (iter);  
}

/*
    ��������� ������
*/

PrimitiveProxy PrimitiveManager::GetPrimitiveProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    ����� ������������ ���������
*/

PrimitivePtr PrimitiveManager::FindPrimitive (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    ��������� ��������� �� ���������
*/

void PrimitiveManager::SetDefaultPrimitive (const PrimitivePtr& primitive)
{
  impl->proxy_manager.SetDefaultResource (primitive);
}

PrimitivePtr PrimitiveManager::DefaultPrimitive ()
{
  return impl->proxy_manager.DefaultResource ();
}
