#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: SetId - register as named material

namespace
{

/*
    Константы
*/

const char* DEFAULT_PROGRAM_NAME = ""; //имя программы по умолчанию

/*
    Текстурная карта
*/

struct Texmap: public xtl::reference_counter, public CacheHolder
{
  MaterialImpl&           material;              //обратная ссылка на материал
  TextureProxy            texture;               //прокси текстуры
  SamplerProxy            sampler;               //сэмплер текстуры
  bool                    is_dynamic;            //является ли текстура динамической  
  TexturePtr              cached_texture;        //закэшированная текстура
  LowLevelSamplerStatePtr cached_sampler;        //закэшированный сэмплер
  LowLevelTexturePtr      cached_device_texture; //закэшированная текстура
  
  Texmap (MaterialImpl& in_material, const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy, bool in_is_dynamic)
    : material (in_material)
    , texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
    , is_dynamic (in_is_dynamic)
  {
    material.AttachCacheSource (*this);
    
    texture.AttachCacheHolder (*this);
    sampler.AttachCacheHolder (*this);
  }
  
  void ResetCacheCore ()
  {
    cached_sampler        = LowLevelSamplerStatePtr ();
    cached_device_texture = LowLevelTexturePtr ();
    cached_texture        = TexturePtr ();
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      cached_texture        = is_dynamic ? TexturePtr () : texture.Resource ();
      cached_device_texture = cached_texture ? cached_texture->DeviceTexture () : LowLevelTexturePtr (); 
      cached_sampler        = sampler.Resource ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::Texmap::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;
};

}

/*
    Описание реализации материала
*/

typedef xtl::intrusive_ptr<Texmap> TexmapPtr;
typedef stl::vector<TexmapPtr>     TexmapArray;
typedef stl::vector<size_t>        TagHashArray;

struct MaterialImpl::Impl: public CacheHolder
{
  DeviceManagerPtr           device_manager;       //менеджер устройства отрисовки
  TextureManagerPtr          texture_manager;      //менеджер текстур
  ShadingManagerPtr          shading_manager;      //менеджер шэйдинга
  stl::string                id;                   //идентификатор материала
  TagHashArray               tags;                 //тэги материала
  ProgramProxy               program;              //прокси программы
  PropertyBuffer             properties;           //свойства материала
  ProgramParametersLayoutPtr properties_layout;    //расположение свойств материала
  TexmapArray                texmaps;              //текстурные карты
  bool                       has_dynamic_textures; //есть ли в материале динамические текстуры
  LowLevelProgramPtr         cached_program;       //закэшированная программа
  LowLevelBufferPtr          cached_properties;    //закэшированный буфер констант
  LowLevelStateBlockPtr      cached_state_block;   //закэшированный блок состояний
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ShadingManagerPtr& in_shading_manager)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , shading_manager (in_shading_manager)
    , program (shading_manager->GetProgramProxy (DEFAULT_PROGRAM_NAME))
    , properties (in_device_manager)
    , has_dynamic_textures (false)
  {
    AttachCacheSource (properties);
  }
  
///Работа с кэшем
  void ResetCacheCore ()
  {
    cached_program    = LowLevelProgramPtr ();
    cached_properties = LowLevelBufferPtr ();
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      cached_program    = program.Resource ();
      cached_properties = properties.Buffer ();
      
      render::low_level::IDevice& device = device_manager->Device ();
      
      device.SSSetProgram (cached_program.get ());
      device.SSSetConstantBuffer (ProgramParametersSlot_Material, cached_properties.get ());
      
      if (!cached_state_block)
      {
        StateBlockMask mask;
        
        mask.ss_program                                           = true;
        mask.ss_constant_buffers [ProgramParametersSlot_Material] = true;
        
        cached_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);        
        
        for (size_t i=0, count=stl::min (texmaps.size (), DEVICE_SAMPLER_SLOTS_COUNT); i<count; i++)
        {
          Texmap& texmap = *texmaps [i];
            
          texmap.UpdateCache ();

          if (texmap.cached_device_texture)
          {
            device.SSSetTexture (i, texmap.cached_device_texture.get ());
            
            mask.ss_textures [i] = true;
          }
          
          device.SSSetSampler (i, texmap.cached_sampler.get ());
          
          mask.ss_samplers [i] = true;
        }
          
        cached_state_block = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
      }
      
      cached_state_block->Capture ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::MaterialImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
};

/*
    Конструктор / деструктор
*/

MaterialImpl::MaterialImpl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ShadingManagerPtr& shading_manager)
  : impl (new Impl (device_manager, texture_manager, shading_manager))
{
  AttachCacheSource (*impl);
}

MaterialImpl::~MaterialImpl ()
{
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
    Тэги
*/

size_t MaterialImpl::TagsCount ()
{
  impl->UpdateCache ();
  
  return impl->tags.size ();
}

const size_t* MaterialImpl::Tags ()
{
  impl->UpdateCache ();
  
  if (impl->tags.empty ())
    return 0;
    
  return &impl->tags [0];
}

/*
    Блок состояний материала
*/

LowLevelStateBlockPtr MaterialImpl::StateBlock ()
{
  impl->UpdateCache ();
  
  return impl->cached_state_block;
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
    
  Texmap& texmap = *impl->texmaps [index];  
  
  texmap.UpdateCache ();
    
  return texmap.cached_texture;
}

LowLevelTexturePtr MaterialImpl::DeviceTexture (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::DeviceTexture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];
    
  texmap.UpdateCache ();
  
  return texmap.cached_device_texture;
}

const char* MaterialImpl::TextureName (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::TextureName", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index]->texture.Name ();
}

LowLevelSamplerStatePtr MaterialImpl::Sampler (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::MaterialImpl::Sampler", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];    
  
  texmap.UpdateCache ();
    
  return texmap.cached_sampler;
}

/*
    Есть ли в материале динамические текстуры
*/

bool MaterialImpl::HasDynamicTextures ()
{
  return impl->has_dynamic_textures;
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
    
    bool new_has_dynamic_textures = false;
    
    for (size_t i=0, count=material.TexmapCount (); i<count; i++)
    {
      const media::rfx::Texmap& texmap = material.Texmap (i);
      
      bool is_dynamic_image = impl->texture_manager->IsDynamicTexture (texmap.Image ());

      if (is_dynamic_image)
        new_has_dynamic_textures = true;
      
      TexmapPtr new_texmap (new Texmap (*this, impl->texture_manager->GetTextureProxy (texmap.Image ()),
        impl->texture_manager->GetSamplerProxy (texmap.Sampler ()), is_dynamic_image), false);

      new_texmaps.push_back (new_texmap);
      
      new_properties.SetProperty (texmap.Semantic (), (int)i); //установка номера канала текстурирования
    }

    ProgramProxy               new_program = impl->shading_manager->GetProgramProxy (material.Program ());
    ProgramParametersLayoutPtr new_layout  = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_Material, new_properties.Layout ());

    new_program.AttachCacheHolder (*impl);

    impl->program.DetachCacheHolder (*impl);

    impl->program = new_program;

    impl->properties.SetProperties (new_properties);

    impl->texmaps.swap (new_texmaps);

    impl->properties_layout    = new_layout;
    impl->cached_state_block   = LowLevelStateBlockPtr ();
    impl->has_dynamic_textures = new_has_dynamic_textures;
    
    Invalidate ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialImpl::Update");
    throw;
  }
}

/*
    Получение объекта расположения параметров программы шейдинга
*/

ProgramParametersLayoutPtr MaterialImpl::ParametersLayout ()
{
  return impl->properties_layout;
}
