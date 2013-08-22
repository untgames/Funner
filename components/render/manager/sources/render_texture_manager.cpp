#include "shared.h"

using namespace render::manager;

namespace
{

/*
    Константы
*/

const char*  DYNAMIC_TEXTURE_NAME_PREFIX        = "dynamic:";
const size_t DYNAMIC_TEXTURE_NAME_PREFIX_LENGTH = strlen (DYNAMIC_TEXTURE_NAME_PREFIX);

/*
    Описание динамической текстуры
*/

struct DynamicTextureDesc: public xtl::reference_counter
{
  stl::string                           name_mask; //маска имени
  TextureManager::DynamicTextureCreator creator;   //функтор создания текстуры
  
  DynamicTextureDesc (const char* in_name_mask, const TextureManager::DynamicTextureCreator& in_creator)
    : name_mask (in_name_mask)
    , creator (in_creator)
  {
  }
};

typedef xtl::intrusive_ptr<DynamicTextureDesc> DynamicTextureDescPtr;
typedef stl::list<DynamicTextureDescPtr>       DynamicTextureDescList;

}

/*
    Описание реализации менеджера текстур
*/

typedef stl::hash_map<stl::hash_key<const char*>, TextureProxy> ProxyMap;

struct TextureManager::Impl
{
  DeviceManagerPtr        device_manager;             //менеджер устройства отрисовки
  TextureProxyManager     texture_proxy_manager;      //менеджер прокси текстур
  TextureDescProxyManager texture_desc_proxy_manager; //менеджер прокси описателей текстур  
  SamplerProxyManager     sampler_proxy_manager;      //менеджер прокси сэмплеров
  ProxyMap                loaded_textures;            //загруженные текстуры
  DynamicTextureDescList  dynamic_texture_descs;      //реестр дескрипторов динамических текстур  
  Log                     log;                        //протокол сообщений  
  
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

TextureManager::TextureManager (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

TextureManager::~TextureManager ()
{
}

/*
    Создание текстур
*/

TexturePtr TextureManager::CreateTexture (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    LowLevelTextureDescPtr desc = FindTextureDesc (name);
    
    if (desc)
    {
      return TexturePtr (new TextureImpl (impl->device_manager, *desc, name), false);
    }
    else
    {
      if (media::CompressedImageManager::FindLoader (name, common::SerializerFindMode_ByName))
      {
        media::CompressedImage image (name);
        
        return CreateTexture (image, name);
      }
      else
      {
        media::Image image (name);

        return CreateTexture (image, true, name);
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::CreateTexture(const char*)");
    throw;
  }
}

TexturePtr TextureManager::CreateTexture (const media::Image& image, bool generate_mips_enable, const char* name)
{
  try
  {
    TextureDimension dimension;
    
    switch (image.Depth ())
    {
      case 1:
        dimension = TextureDimension_2D;
        break;
      case 6:
        dimension = TextureDimension_Cubemap;
        break;
      default:
        dimension = TextureDimension_3D;
        break;
    }

    return CreateTexture (image, dimension, generate_mips_enable, name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::CreateTexture(const media::Image&,bool)");
    throw;
  }
}

TexturePtr TextureManager::CreateTexture (const media::Image& image, render::manager::TextureDimension dimension, bool generate_mips_enable, const char* name)
{
  try
  {
    PixelFormat format;
    
    switch (image.Format ())
    {
      case media::PixelFormat_BGR8:
      case media::PixelFormat_RGB16:
        impl->log.Printf ("Convert image '%s' from '%s' to '%s'", image.Name (), media::get_format_name (image.Format ()), get_name (PixelFormat_RGB8));
      case media::PixelFormat_RGB8:
        format = PixelFormat_RGB8;
        break;
      case media::PixelFormat_BGRA8:
      case media::PixelFormat_RGBA16:
        impl->log.Printf ("Convert image '%s' from '%s' to '%s'", image.Name (), media::get_format_name (image.Format ()), get_name (PixelFormat_RGBA8));
      case media::PixelFormat_RGBA8:
        format = PixelFormat_RGBA8;
        break;
      case media::PixelFormat_L8:
        format = PixelFormat_L8;
        break;
      case media::PixelFormat_A8:
        format = PixelFormat_A8;
        break;
      case media::PixelFormat_LA8:
        format = PixelFormat_LA8;
        break;
      default:
        throw xtl::format_not_supported_exception ("", "Unsupported image '%s' format '%s'", image.Name (), media::get_format_name (image.Format ()));
    }

    TexturePtr texture = CreateTexture (dimension, image.Width (), image.Height (), image.Depth (), format, generate_mips_enable, name);

    texture->Update (image);

    return texture;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::CreateTexture(const media::Image&,TextureDimension,bool)");
    throw;
  }
}

TexturePtr TextureManager::CreateTexture (const media::CompressedImage& image, const char* name)
{
  try
  {
    TextureDimension dimension;
    
    switch (image.LayersCount ())
    {
      case 1:
        dimension = TextureDimension_2D;
        break;
      case 6:
        dimension = TextureDimension_Cubemap;
        break;
      default:
        dimension = TextureDimension_3D;
        break;
    }

    return CreateTexture (image, dimension, name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::CreateTexture(const media::CompressedImage&)");
    throw;
  }
}

TexturePtr TextureManager::CreateTexture (const media::CompressedImage& image, render::manager::TextureDimension dimension, const char* name)
{
  try
  {
    return TexturePtr (new TextureImpl (impl->device_manager, dimension, image, name), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::CreateTexture(const media::CompressedImage&,TextureDimension)");
    throw;
  }
}

TexturePtr TextureManager::CreateTexture
 (render::manager::TextureDimension dimension,
  size_t                            width,
  size_t                            height,
  size_t                            depth,
  render::manager::PixelFormat      format,
  bool                              generate_mips_enable,
  const char*                       name)
{
  try
  {
    return TexturePtr (new TextureImpl (impl->device_manager, dimension, width, height, depth, format, generate_mips_enable, name), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::CreateTexture(TextureDimension,size_t,size_t,size_t,PixelFormat,bool)");
    throw;
  }
}

/*
    Проверка: является ли ресурс текстурой
*/

bool TextureManager::IsTextureResource (const char* name)
{
  return name && (media::ImageManager::FindLoader (name, common::SerializerFindMode_ByName) != 0 || media::CompressedImageManager::FindLoader (name, common::SerializerFindMode_ByName) != 0);
}

/*
    Загрузка / выгрузка текстур
*/

void TextureManager::LoadTexture (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    impl->log.Printf ("Loading texture '%s'", name);
      
    if (impl->loaded_textures.find (name) != impl->loaded_textures.end ())
      throw xtl::format_operation_exception ("", "Texture '%s' has been already loaded", name);            
      
    TexturePtr texture;

    if (media::CompressedImageManager::FindLoader (name, common::SerializerFindMode_ByName))
    {
      media::CompressedImage image (name);
      
      texture = CreateTexture (image, name);
    }
    else
    {
      media::Image image (name);

      texture = CreateTexture (image, true, name);
    }

    TextureProxy proxy = impl->texture_proxy_manager.GetProxy (name);

    proxy.SetResource (texture);

    impl->loaded_textures.insert_pair (name, proxy);
    
    if (impl->device_manager->Settings ().HasDebugLog ())
      impl->log.Printf ("...created texture %ux%ux%u@%s", texture->Width (), texture->Height (), texture->Depth (), get_name (texture->Format ()));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::LoadTexture");
    throw;
  }
}

void TextureManager::ShareTexture (const char* name, const TexturePtr& texture)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");
      
    impl->log.Printf ("Share texture '%s'", name);
      
    if (impl->loaded_textures.find (name) != impl->loaded_textures.end ())
      throw xtl::format_operation_exception ("", "Texture '%s' has been already loaded", name);
      
    TextureProxy proxy = impl->texture_proxy_manager.GetProxy (name);

    proxy.SetResource (texture);

    impl->loaded_textures.insert_pair (name, proxy);
    
    if (impl->device_manager->Settings ().HasDebugLog ())
      impl->log.Printf ("...shared texture %ux%ux%u@%s", texture->Width (), texture->Height (), texture->Depth (), get_name (texture->Format ()));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::TextureManager::ShareTexture");
    throw;
  }
}

void TextureManager::UnloadTexture (const char* name)
{
  if (!name)
    return;
    
  ProxyMap::iterator iter = impl->loaded_textures.find (name);
  
  if (iter == impl->loaded_textures.end ())
    return;    
    
  iter->second.SetResource (TexturePtr ());
  iter->second.ResetCache ();
    
  impl->loaded_textures.erase (iter);
}

/*
    Получение прокси
*/

TextureProxy TextureManager::GetTextureProxy (const char* name)
{
  return impl->texture_proxy_manager.GetProxy (name);
}

TextureDescProxy TextureManager::GetTextureDescProxy (const char* name)
{
  return impl->texture_desc_proxy_manager.GetProxy (name);
}

SamplerProxy TextureManager::GetSamplerProxy (const char* name)
{
  return impl->sampler_proxy_manager.GetProxy (name);
}

/*
    Поиск загруженной текстуры / сэмплера
*/

TexturePtr TextureManager::FindTexture (const char* name)
{
  return impl->texture_proxy_manager.FindResource (name);
}

LowLevelTextureDescPtr TextureManager::FindTextureDesc (const char* name)
{
  return impl->texture_desc_proxy_manager.FindResource (name);
}

LowLevelSamplerStatePtr TextureManager::FindSampler (const char* name)
{
  return impl->sampler_proxy_manager.FindResource (name);
}

/*
    Создание динамической текстуры
*/

DynamicTexturePtr TextureManager::CreateDynamicTexture (const char* name, EntityImpl& entity)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!entity)
      throw xtl::make_null_argument_exception ("", "entity");
    
    for (DynamicTextureDescList::iterator iter=impl->dynamic_texture_descs.begin (), end=impl->dynamic_texture_descs.end (); iter!=end; ++iter)
      if (common::wcmatch (name, (*iter)->name_mask.c_str ()))        
        return DynamicTexturePtr (new DynamicTextureImpl (name, entity, (*iter)->creator), false);

    throw xtl::make_argument_exception ("", "Dynamic texture creator for '%s' not registered", name);
  }
  catch (xtl::exception& e)
  {
    e.touch (name ? "render::manager::TextureManager::CreateDynamicTexture('%s')" : "render::manager::TextureManager::CreateDynamicTexture", name);
    throw;
  }
}

/*
    Регистрация динамических текстур
*/

void TextureManager::RegisterDynamicTexture (const char* name_mask, const DynamicTextureCreator& creator)
{
  static const char* METHOD_NAME = "render::manager::TextureManager::RegisterDynamicTexture";

  if (!name_mask)
    throw xtl::make_argument_exception (METHOD_NAME, "name_mask");
    
  impl->dynamic_texture_descs.push_back (DynamicTextureDescPtr (new DynamicTextureDesc (name_mask, creator), false));  
}

void TextureManager::UnregisterDynamicTexture (const char* name_mask)
{
  if (!name_mask)
    return;
    
  for (DynamicTextureDescList::iterator iter=impl->dynamic_texture_descs.begin (), end=impl->dynamic_texture_descs.end (); iter!=end;)
    if (name_mask == (*iter)->name_mask)
    {
      DynamicTextureDescList::iterator next = iter;
      
      ++next;
      
      impl->dynamic_texture_descs.erase (iter);
      
      iter = next;
    }
    else ++iter;
}

void TextureManager::UnregisterAllDynamicTextures ()
{
  impl->dynamic_texture_descs.clear ();
}

bool TextureManager::IsDynamicTexture (const char* name)
{
  if (!name)
    return false;
  
  return strncmp (name, DYNAMIC_TEXTURE_NAME_PREFIX, DYNAMIC_TEXTURE_NAME_PREFIX_LENGTH) == 0;
}

/*
    Установка текстуры по умолчанию / сэмплера по умолчанию
*/

void TextureManager::SetDefaultTexture (const TexturePtr& texture)
{
  impl->texture_proxy_manager.SetDefaultResource (texture);
}

TexturePtr TextureManager::DefaultTexture ()
{
  return impl->texture_proxy_manager.DefaultResource ();
}

void TextureManager::SetDefaultTextureDesc (const LowLevelTextureDescPtr& texture)
{
  impl->texture_desc_proxy_manager.SetDefaultResource (texture);
}

LowLevelTextureDescPtr TextureManager::DefaultTextureDesc ()
{
  return impl->texture_desc_proxy_manager.DefaultResource ();
}

void TextureManager::SetDefaultSampler (const LowLevelSamplerStatePtr& sampler)
{
  impl->sampler_proxy_manager.SetDefaultResource (sampler);
}

LowLevelSamplerStatePtr TextureManager::DefaultSampler ()
{
  return impl->sampler_proxy_manager.DefaultResource ();
}
