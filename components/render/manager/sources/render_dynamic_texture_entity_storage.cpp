#include "shared.h"

using namespace render;

/*
    Описание реализации хранилища динамических текстур объекта
*/

typedef stl::hash_map<stl::hash_key<const char*>, DynamicTexturePtr> DynamicTextureMap;

struct DynamicTextureEntityStorage::Impl
{
  DynamicTextureMap dynamic_textures; //карта динамических текстур
  Log               log;              //протокол рендеринга
};

/*
    Конструктор / деструктор
*/

DynamicTextureEntityStorage::DynamicTextureEntityStorage ()
  : impl (new Impl)
{
}

DynamicTextureEntityStorage::~DynamicTextureEntityStorage ()
{
}

/*
    Обновление текстур
*/

void DynamicTextureEntityStorage::Update (const FramePtr& frame)
{
  try
  {
    if (!frame)
      throw xtl::make_null_argument_exception ("", "frame");
    
    for (DynamicTextureMap::iterator iter=impl->dynamic_textures.begin (), end=impl->dynamic_textures.end (); iter!=end; ++iter)
    {
      DynamicTexturePtr texture = iter->second;
      
      try
      {     
        texture->Update (frame);
      }
      catch (std::exception& e)
      {
        impl->log.Printf ("Error at update dynamic texture '%s': %s", texture->Name (), e.what ());
      }
      catch (...)
      {
        impl->log.Printf ("Unknown error at update dynamic texture '%s'", texture->Name ());
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::DynamicTextureEntityStorage::Update");
    throw;
  }
}

/*
    Добавление / поиск динамических текстур
*/

void DynamicTextureEntityStorage::AddTexture (const char* name, const DynamicTexturePtr& dynamic_texture)
{
  static const char* METHOD_NAME = "render::DynamicTextureEntityStorage::AddTexture";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  if (!dynamic_texture)
    throw xtl::make_null_argument_exception (METHOD_NAME, "dynamic_texture");
    
  DynamicTextureMap::iterator iter = impl->dynamic_textures.find (name);
  
  if (iter != impl->dynamic_textures.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Dynamic texture has already registered for this entity");
    
  impl->dynamic_textures.insert_pair (name, dynamic_texture);
}

DynamicTexturePtr DynamicTextureEntityStorage::FindTexture (const char* name)
{
  if (!name)
    return DynamicTexturePtr ();
    
  DynamicTextureMap::iterator iter = impl->dynamic_textures.find (name);
  
  if (iter == impl->dynamic_textures.end ())
    return DynamicTexturePtr ();
    
  return iter->second;
}

/*
    Сброс неиспользуемых ресурсов
*/

void DynamicTextureEntityStorage::FlushUnusedTextures ()
{
  for (DynamicTextureMap::iterator iter=impl->dynamic_textures.begin (), end=impl->dynamic_textures.end (); iter!=end;)
    if (iter->second->use_count () == 1)
    {
      DynamicTextureMap::iterator next = iter;
      
      ++next;
      
      impl->dynamic_textures.erase (iter);
      
      iter = next;
    }
    else ++iter; 
}
