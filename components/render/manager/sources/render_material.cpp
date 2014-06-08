#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

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
  TextureProxy            texture;               //прокси текстуры
  SamplerProxy            sampler;               //сэмплер текстуры
  bool                    is_dynamic;            //является ли текстура динамической
  size_t                  semantic_hash;         //хеш имени семантики
  TexturePtr              cached_texture;        //закэшированная текстура
  LowLevelSamplerStatePtr cached_sampler;        //закэшированный сэмплер
  LowLevelTexturePtr      cached_device_texture; //закэшированная текстура
  
  Texmap (CacheHolder& owner, const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy, bool in_is_dynamic, const char* semantic)
    : texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
    , is_dynamic (in_is_dynamic)
    , semantic_hash (semantic ? common::strhash (semantic) : 0)
  {
    owner.AttachCacheSource (*this);
    
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
      TexturePtr              new_cached_texture        = is_dynamic ? TexturePtr () : texture.Resource ();                         
      LowLevelSamplerStatePtr new_cached_sampler        = sampler.Resource ();
      LowLevelTexturePtr      new_cached_device_texture = new_cached_texture ? new_cached_texture->DeviceTexture () : LowLevelTexturePtr ();
      
      if (new_cached_texture == cached_texture && new_cached_sampler == cached_sampler && new_cached_device_texture == cached_device_texture)
        return;                      
      
      cached_texture        = new_cached_texture;
      cached_device_texture = new_cached_device_texture;      
      cached_sampler        = new_cached_sampler;
      
      InvalidateCache ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::Texmap::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

}

/*
    Описание реализации материала
*/

typedef xtl::intrusive_ptr<Texmap> TexmapPtr;
typedef stl::vector<TexmapPtr>     TexmapArray;
typedef stl::vector<size_t>        TagHashArray;

struct MaterialImpl::Impl: public CacheHolder, public DebugIdHolder
{
  DeviceManagerPtr           device_manager;               //менеджер устройства отрисовки
  TextureManagerPtr          texture_manager;              //менеджер текстур
  ProgramManagerPtr          program_manager;              //менеджер программ
  stl::string                name;                         //имя материала
  TagHashArray               tags;                         //тэги материала
  ProgramProxy               program;                      //прокси программы
  PropertyBuffer             properties;                   //свойства материала
  ProgramParametersLayoutPtr material_properties_layout;   //расположение свойств материала
  TexmapArray                texmaps;                      //текстурные карты
  bool                       has_dynamic_textures;         //есть ли в материале динамические текстуры
  size_t                     cached_state_block_mask_hash; //хэш закэшированной маски блока состояний материала
  ProgramPtr                 cached_program;               //закэшированная программа
  LowLevelStateBlockPtr      cached_state_block;           //закэшированный блок состояний
  ProgramParametersLayoutPtr cached_properties_layout;     //расположение свойств материала и программы
  Log                        log;                          //протокол отладочных сообщений
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ProgramManagerPtr& in_program_manager, const char* in_name)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , program_manager (in_program_manager)
    , program (program_manager->GetProgramProxy (DEFAULT_PROGRAM_NAME))
    , properties (in_device_manager)
    , has_dynamic_textures (false)
    , cached_state_block_mask_hash (0)
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("render::manager::MaterialImpl::Impl::Impl", "name");
      
    name = in_name;
    
    AttachCacheSource (properties);
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Material '%s' created (id=%u)", name.c_str (), Id ());
  }
  
///Деструктор
  ~Impl ()
  {
      //предварительная очистка коллекция для возможности отслеживать порядок удаления объектов до и после удаления данного
      
    cached_program               = ProgramPtr ();
    cached_properties_layout     = ProgramParametersLayoutPtr ();
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Material '%s' destroyed (id=%u)", name.c_str (), Id ());
  }
  
///Работа с кэшем
  void ResetCacheCore ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset material cache (id=%u)", Id ());

    cached_properties_layout     = ProgramParametersLayoutPtr ();
    cached_program               = ProgramPtr ();
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      bool need_invalidate_deps = false; //флаг необходимости обновления зависимых от материала кэшей
      
      bool has_debug_log = device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update material '%s' cache (id=%u)", name.c_str (), Id ());
        
        //кэширование программы рендеринга

      ProgramPtr old_program = cached_program;

      cached_program       = program.Resource ();
      need_invalidate_deps = need_invalidate_deps || old_program != cached_program;
      
      if (cached_program)
      {
        ProgramParametersLayoutPtr old_layout = cached_properties_layout;
        
        cached_properties_layout = device_manager->ProgramParametersManager ().GetParameters (&*material_properties_layout, &*cached_program->ParametersLayout (), 0);
        need_invalidate_deps     = need_invalidate_deps || old_layout != cached_properties_layout;
      }
      else throw xtl::format_operation_exception ("", "Null program for material '%s' (id=%u)", name.c_str (), Id ());
      
        //кэширование константного буфера материала
      
      LowLevelBufferPtr device_properties = properties.Buffer ();      
      
      render::low_level::IDevice&        device  = device_manager->Device ();
      render::low_level::IDeviceContext& context = device_manager->ImmediateContext ();
      
        //установка константного буфера и сброс программы рендеринга в устройстве отрисовки

      context.SSSetConstantBuffer (ProgramParametersSlot_Material, device_properties.get ());
      context.SSSetConstantBuffer (ProgramParametersSlot_Program, 0);
      
        //применение настроек программы к состоянию устройства отрисовки (для объединения их с настройками материала)
        
      StateBlockMask mask;        

      if (cached_program)
      {
        LowLevelStateBlockPtr program_state_block = cached_program->StateBlock ();

        if (program_state_block)
        {
            //получение маски флагов программы рендеринга

          program_state_block->GetMask (mask);

            //применение настроек программы рендеринга

          program_state_block->Apply (&context);
        }
      }

        //определение настроек материала

      mask.ss_constant_buffers [ProgramParametersSlot_Material] = true;
      mask.ss_constant_buffers [ProgramParametersSlot_Program]  = true;

        //установка статических текстурных карт и их сэмплеров в контекст устройства отрисовки

      if (cached_program)
      {
        const TexmapDesc* program_texmap = cached_program->Texmaps ();

        for (size_t i = 0, count = cached_program->TexmapsCount (); i < count; i++, program_texmap++)
        {
          size_t channel = program_texmap->channel;

          if (channel >= DEVICE_SAMPLER_SLOTS_COUNT)
            continue;

          Texmap* texmap = 0;

          for (TexmapArray::iterator iter = texmaps.begin (), end = texmaps.end (); iter != end; ++iter)
          {
            if ((*iter)->semantic_hash == program_texmap->semantic_hash)
            {
              texmap = &**iter;
              break;
            }
          }

          mask.ss_textures [channel] = !texmap || !texmap->is_dynamic;
          mask.ss_samplers [channel] = true;

          if (texmap)
          {
            if (!texmap->cached_device_texture)
              log.Printf ("Texmap[%u] in program '%s' for material '%s' will be ignored. Bad texture '%s'", channel, cached_program->Name (), name.c_str (), texmap->texture.Name ());

            if (!texmap->cached_sampler)
              log.Printf ("Texmap[%u] in program '%s' for material '%s' will be ignored. Bad sampler '%s'", channel, cached_program->Name (), name.c_str (), texmap->sampler.Name ());

            context.SSSetTexture (channel, texmap->cached_device_texture.get ());
            context.SSSetSampler (channel, texmap->cached_sampler.get ());
          }
          else
          {
            context.SSSetTexture (channel, 0);
            context.SSSetSampler (channel, 0);
          }
        }
      }

        //проверка необходимости пересоздания блока состояний материала

      size_t state_block_mask_hash = mask.Hash ();

      if (!cached_state_block || cached_state_block_mask_hash != state_block_mask_hash)
      {
        if (has_debug_log)
          log.Printf ("...create state block for material");

        cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
        cached_state_block_mask_hash = state_block_mask_hash;
        need_invalidate_deps         = true;
      }

        //сохранение состояния контекста устройства отрисовки

      cached_state_block->Capture (&context);

        //обновление зависимых кэшей

      if (need_invalidate_deps)
        InvalidateCacheDependencies ();

      if (has_debug_log)
        log.Printf ("...material cache updated");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::MaterialImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;  
  using CacheHolder::ResetCache;
  using CacheHolder::InvalidateCache;
};

/*
    Конструктор / деструктор
*/

MaterialImpl::MaterialImpl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ProgramManagerPtr& program_manager, const char* name)
  : impl (new Impl (device_manager, texture_manager, program_manager, name))
{
  AttachCacheSource (*impl);
}

MaterialImpl::~MaterialImpl ()
{
}

/*
    Имя
*/

const char* MaterialImpl::Name ()
{
  return impl->name.c_str ();
}

void MaterialImpl::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::MaterialImpl::SetName", "name");
    
  if (impl->device_manager->Settings ().HasDebugLog ())
    impl->log.Printf ("Material '%s' name changed to '%s' (id=%u)", impl->name.c_str (), name, impl->Id ());
    
  impl->name = name;
}

/*
    Тэги
*/

size_t MaterialImpl::TagsCount ()
{
  UpdateCache ();
  
  return impl->tags.size ();
}

const size_t* MaterialImpl::Tags ()
{
  UpdateCache ();
  
  if (impl->tags.empty ())
    return 0;
    
  return &impl->tags [0];
}

/*
    Блок состояний материала
*/

LowLevelStateBlockPtr MaterialImpl::StateBlock ()
{
  UpdateCache ();
  
  return impl->cached_state_block;
}

/*
    Программа шейдинга
*/

ProgramPtr MaterialImpl::Program ()
{
  return impl->cached_program;
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
    throw xtl::make_range_exception ("render::manager::MaterialImpl::Texture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];  
  
  texmap.UpdateCache ();
    
  return texmap.cached_texture;
}

LowLevelTexturePtr MaterialImpl::DeviceTexture (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::DeviceTexture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];
    
  texmap.UpdateCache ();
  
  return texmap.cached_device_texture;
}

const char* MaterialImpl::TextureName (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::TextureName", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index]->texture.Name ();
}

LowLevelSamplerStatePtr MaterialImpl::Sampler (size_t index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::Sampler", "index", index, impl->texmaps.size ());
    
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
      //сохранение свойств материала, получение прокси-программы материала
    
    common::PropertyMap new_properties = material.Properties ();
    ProgramProxy        new_program    = impl->program_manager->GetProgramProxy (material.Program ());    
    
      //создание текстурных карт
    
    TexmapArray new_texmaps;

    new_texmaps.reserve (material.TexmapCount ());
    
    bool new_has_dynamic_textures = false;
    
    for (size_t i=0, count=material.TexmapCount (); i<count; i++)
    {
      const media::rfx::Texmap& texmap = material.Texmap (i);
      
        //определение является ли текстура динамической производится по префиксу её имени и потому может быть выполнено однократно
      
      bool is_dynamic_image = impl->texture_manager->IsDynamicTexture (texmap.Image ());

      if (is_dynamic_image)
        new_has_dynamic_textures = true;        
      
      TexmapPtr new_texmap (new Texmap (*impl, impl->texture_manager->GetTextureProxy (texmap.Image ()),
        impl->texture_manager->GetSamplerProxy (texmap.Sampler ()), is_dynamic_image, texmap.Semantic ()), false);

      new_texmaps.push_back (new_texmap);
    }
    
      //копирование тэгов материала
    
    TagHashArray new_tag_hashes (material.TagHashes (), material.TagHashes () + material.TagsCount ());
    
    if (new_tag_hashes.empty ())
      impl->log.Printf ("Warning: material '%s' has no tags. Will not be displayed", material.Name ());
      
      //получение объекта расположения свойств материала
    
    ProgramParametersLayoutPtr new_layout = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_Material, new_properties.Layout ());
    
      //регистрация обновлений

    if (new_program != impl->program)
    {
      new_program.AttachCacheHolder (*impl);

      impl->program.DetachCacheHolder (*impl);
      
      impl->program = new_program;      
    }

    impl->properties.SetProperties (new_properties);

    impl->texmaps.swap (new_texmaps);
    impl->tags.swap (new_tag_hashes);

    impl->material_properties_layout = new_layout;
    impl->has_dynamic_textures       = new_has_dynamic_textures;
    
      //обновление кэша с зависимостями (поскольку может измениться состояние тэгов и динамических текстур)
      
    impl->InvalidateCache ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::MaterialImpl::Update");
    throw;
  }
}

/*
    Получение объекта расположения параметров программы шейдинга
*/

ProgramParametersLayoutPtr MaterialImpl::ParametersLayout ()
{
  try
  {
    UpdateCache ();
    
    return impl->cached_properties_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::MaterialImpl::ParametersLayout");
    throw;
  }
}

/*
    Управление кэшированием
*/

void MaterialImpl::UpdateCacheCore ()
{
    //обновление зависимых кэшей (реакция на Impl::InvalidateCacheDependencies)

  InvalidateCacheDependencies ();
}

void MaterialImpl::ResetCacheCore ()
{
  impl->ResetCache ();
  
  for (TexmapArray::iterator iter=impl->texmaps.begin (), end=impl->texmaps.end (); iter!=end; ++iter)
    (*iter)->ResetCache ();
}
