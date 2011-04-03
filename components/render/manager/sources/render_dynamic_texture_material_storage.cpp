#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: optimize perfomance of UpdateCacheCore

namespace
{

/*
    Канал с динамической текстурой
*/

struct DynamicTextureChannel: public xtl::reference_counter
{
  DynamicTexturePtr dynamic_texture; //динамическая текстура
  size_t            channel_index;   //номер канала
};

typedef xtl::intrusive_ptr<DynamicTextureChannel> DynamicTextureChannelPtr;
typedef stl::vector<DynamicTextureChannelPtr>     DynamicTextureChannelArray;

}

/*
    Описание реализации хранилища динамических текстур материала
*/

struct DynamicTextureMaterialStorage::Impl
{
  TextureManagerPtr                 texture_manager;  //менеджер текстур
  EntityImpl&                       owner;            //владелец хранилища текстур
  MaterialPtr                       material;         //материал
  DynamicTextureChannelArray        dynamic_textures; //динамические текстуры
  render::low_level::StateBlockMask state_block_mask; //маска блока состояний
  
///Конструктор
  Impl (const TextureManagerPtr& in_texture_manager, const MaterialPtr& in_material, EntityImpl& in_owner)
    : texture_manager (in_texture_manager)
    , material (in_material)
    , owner (in_owner)
  {
    static const char* METHOD_NAME = "render::DynamicTextureMaterialStorage::Impl::Impl";
    
    if (!material)
      throw xtl::make_null_argument_exception (METHOD_NAME, "material");
      
    if (!texture_manager)
      throw xtl::make_null_argument_exception (METHOD_NAME, "texture_manager");          
  }
};

/*
    Конструктор / деструктор
*/

DynamicTextureMaterialStorage::DynamicTextureMaterialStorage (const TextureManagerPtr& texture_manager, const MaterialPtr& material, EntityImpl& entity)
  : impl (new Impl (texture_manager, material, entity))
{
  AttachCacheSource (*material);
}

DynamicTextureMaterialStorage::~DynamicTextureMaterialStorage ()
{
}

/*
    Получение маски блока состояний
*/

void DynamicTextureMaterialStorage::UpdateMask (render::low_level::StateBlockMask& mask)
{
  try
  {
    UpdateCache ();
    
    mask |= impl->state_block_mask;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::DynamicTextureMaterialStorage::UpdateMask");
    throw;
  }
}

/*
    Применение текстур материала
*/

void DynamicTextureMaterialStorage::Apply (render::low_level::IDevice& device)
{
  try
  {
    UpdateCache ();
    
    for (DynamicTextureChannelArray::iterator iter=impl->dynamic_textures.begin (), end=impl->dynamic_textures.end (); iter!=end; ++iter)
    {
      DynamicTextureChannel& channel = **iter;
      TexturePtr             texture = channel.dynamic_texture->Texture ();
      
      if (!texture)
        throw xtl::format_operation_exception ("", "Null dynamic texture at channel %u", channel.channel_index);
      
      device.SSSetTexture (channel.channel_index, texture ? texture->DeviceTexture ().get () : (render::low_level::ITexture*)0);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::DynamicTextureMaterialStorage::Apply");
    throw;
  }
}

/*
    Обновление кэша
*/

void DynamicTextureMaterialStorage::ResetCacheCore ()
{
  impl->dynamic_textures.clear ();  
}

void DynamicTextureMaterialStorage::UpdateCacheCore ()
{
  try
  {
      //TODO: optimize perfomance
    
    MaterialImpl& material = *impl->material;
    
    size_t dynamic_textures_count = 0;
    
    for (size_t i=0, count=material.TexturesCount (); i<count; i++)
    {
      if (material.Texture (i))
        continue;
        
      dynamic_textures_count++;
    }
    
    impl->dynamic_textures.reserve (dynamic_textures_count);
    
    DynamicTextureEntityStorage& cache = impl->owner.DynamicTextureStorage ();
    
    StateBlockMask mask;
    
    try
    {
      for (size_t i=0, count=stl::min (material.TexturesCount (), DEVICE_SAMPLER_SLOTS_COUNT); i<count; i++)
      {
        if (material.Texture (i))
          continue;                
        
        const char* name = material.TextureName (i);
        
        DynamicTexturePtr dynamic_texture = cache.FindTexture (name);
        
        if (!dynamic_texture)
        {
          dynamic_texture = impl->texture_manager->CreateDynamicTexture (name, impl->owner);
          
          cache.AddTexture (name, dynamic_texture);
        }

        DynamicTextureChannelPtr dynamic_texture_channel (new DynamicTextureChannel, false);
        
        dynamic_texture_channel->dynamic_texture = dynamic_texture;
        dynamic_texture_channel->channel_index   = i;
        
        mask.ss_textures [i] = true;
        
        impl->dynamic_textures.push_back (dynamic_texture_channel);
      }
      
      impl->state_block_mask = mask;
    }
    catch (...)
    {
      impl->dynamic_textures.clear ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::DynamicTextureMaterialStorage::UpdateCacheCore");
    throw;
  }
}
