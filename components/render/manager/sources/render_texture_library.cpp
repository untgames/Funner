#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера текстур
*/

typedef stl::hash_map<stl::hash_key<const char*>, TextureProxy> ProxyMap;

struct TextureLibrary::Impl
{
  RenderManagerImpl&  manager;          //ссылка на владельца
  TextureProxyManager proxy_manager;    //менеджер прокси объектов
  ProxyMap            loaded_textures;  //загруженные текстуры
  
  Impl (RenderManagerImpl& in_manager)
    : manager (in_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

TextureLibrary::TextureLibrary (RenderManagerImpl& owner)
  : impl (new Impl (owner))
{
}

TextureLibrary::~TextureLibrary ()
{
}

/*
    Проверка: является ли ресурс текстурой
*/

bool TextureLibrary::IsTextureResource (const char* name)
{
  return name && media::ImageManager::FindLoader (name, common::SerializerFindMode_ByName) != 0;
}

/*
    Загрузка / выгрузка текстур
*/

void TextureLibrary::LoadTexture (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (impl->loaded_textures.find (name) != impl->loaded_textures.end ())
      throw xtl::format_operation_exception ("", "Texture '%s' has been already loaded", name);
    
    media::Image image (name);
    
    TexturePtr   texture = impl->manager.CreateTexture (image, true);    
    TextureProxy proxy   = impl->proxy_manager.GetProxy (name);
    
    proxy.SetResource (texture);
    
    impl->loaded_textures.insert_pair (name, proxy);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::TextureLibrary::LoadTexture");
    throw;
  }
}

void TextureLibrary::UnloadTexture (const char* name)
{
  if (!name)
    return;
    
  impl->loaded_textures.erase (name);
}

/*
    Получение прокси
*/

TextureProxy TextureLibrary::GetTextureProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    Поиск загруженной текстуры
*/

TexturePtr TextureLibrary::FindTexture (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    Установка текстуры по умолчанию
*/

void TextureLibrary::SetDefaultTexture (const TexturePtr& texture)
{
  impl->proxy_manager.SetDefaultResource (texture);
}

TexturePtr TextureLibrary::DefaultTexture ()
{
  return impl->proxy_manager.DefaultResource ();
}
