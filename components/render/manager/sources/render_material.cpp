#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: SetId - register as named material
//TODO: dynamic texture

namespace
{

/*
    Текстурная карта
*/

struct Texmap
{
  TextureProxy texture; //прокси текстуры
  SamplerProxy sampler; //сэмплер текстуры
  
  Texmap (const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy)
    : texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
  {
  }
};

}

/*
    Описание реализации материала
*/

typedef stl::vector<Texmap> TexmapArray;

struct MaterialImpl::Impl
{
  TextureManagerPtr   texture_manager; //менеджер текстур
  stl::string         id;              //идентификатор материала
  common::PropertyMap properties;      //свойства материала
  TexmapArray         texmaps;         //текстурные карты
  
///Конструктор
  Impl (const TextureManagerPtr& in_texture_manager)
    : texture_manager (in_texture_manager)
  {
  }
  
///Отсоединение от кэша
  void DetachCache (CacheHolder& holder)
  {
    for (TexmapArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
    {
      iter->texture.Detach (holder);
      iter->sampler.Detach (holder);
    }
  }
  
///Присоединение к кэшу
  void AttachCache (CacheHolder& holder)
  {
    try
    {
      for (TexmapArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
      {
        iter->texture.Attach (holder);
        iter->sampler.Attach (holder);
      }      
    }
    catch (...)
    {
      DetachCache (holder);
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

MaterialImpl::MaterialImpl (const TextureManagerPtr& texture_manager)
  : impl (new Impl (texture_manager))
{
}

MaterialImpl::~MaterialImpl ()
{
  impl->DetachCache (*this);
}

/*
    Идентификатор материала
*/

const char* MaterialImpl::Id ()
{
  return impl->id.c_str ();
}

void MaterialImpl::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("render::MaterialImpl::SetId", "id");
    
  impl->id = id;
}

/*
    Свойства материала
*/

const common::PropertyMap& MaterialImpl::Properties ()
{
  return impl->properties;
}

/*
    Получение текстуры
*/

size_t MaterialImpl::TexturesCount ()
{
  return impl->texmaps.size ();
}

TexturePtr MaterialImpl::Texture (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::Texture", "index", index, impl->texmaps.size ());
    
    //TODO: dynamic texture?????
    
  return impl->texmaps [index].texture.Resource ();
}

const char* MaterialImpl::TextureName (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::TextureName", "index", index, impl->texmaps.size ());

  return impl->texmaps [index].texture.Name ();
}

LowLevelSamplerStatePtr MaterialImpl::Sampler (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::Sampler", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index].sampler.Resource ();
}

/*
    Обновление материала
*/

void MaterialImpl::Update (const media::rfx::Material& material)
{
  try
  {
    common::PropertyMap new_properties = material.Properties ().Clone ();
    
    TexmapArray new_texmaps;
    
    new_texmaps.reserve (material.TexmapCount ());
    
    for (size_t i=0, count=material.TexmapCount (); i<count; i++)
    {
      const media::rfx::Texmap& texmap = material.Texmap (i);
      
      Texmap new_texmap (impl->texture_manager->GetTextureProxy (texmap.Image ()), impl->texture_manager->GetSamplerProxy (texmap.Sampler ()));
      
      new_texmaps.push_back (new_texmap);
    }    
    
    impl->properties = new_properties;
    
    impl->DetachCache (*this);    

    impl->texmaps.swap (new_texmaps);
    
    impl->AttachCache (*this);
    
    Invalidate ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialImpl::Update");
    throw;
  }
}
