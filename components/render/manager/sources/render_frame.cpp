#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

namespace
{

/*
    Константы
*/

const size_t RESERVED_ENTITIES_COUNT = 512; //резервируемое количество отображаемых объектов
const size_t RESERVED_FRAMES_COUNT   = 32;  //резервируемое количество отображаемых фреймов

/*
    Вспомогательные структуры
*/

typedef stl::hash_map<stl::hash_key<const char*>, TexturePtr> TextureMap;

/*
    Хранилище для эффекта
*/

struct EffectHolder: public CacheSource
{
  FrameImpl&               frame;                  //ссылка на кадр
  DeviceManagerPtr         device_manager;         //менеджер устройства отрисовки
  EffectManagerPtr         effect_manager;         //ссылка на менеджер эффектов
  EffectProxy              effect;                 //эффект кадра
  EffectRendererPtr        effect_renderer;        //рендер эффекта
  EffectPtr                cached_effect;          //закэшированный эффект
  ProgramParametersLayout  properties_layout;      //расположение свойств
  
///Конструктор
  EffectHolder (const EffectManagerPtr& in_effect_manager, const DeviceManagerPtr& in_device_manager, FrameImpl& in_frame)
    : frame (in_frame)
    , device_manager (in_device_manager)
    , effect_manager (in_effect_manager)
    , effect (effect_manager->GetEffectProxy (""))
    , properties_layout (&device_manager->Device (), &device_manager->Settings ())
  {
    effect.AttachCacheHolder (*this);
  }
  
///Работа с кэшем
  void ResetCacheCore ()
  {
    cached_effect   = EffectPtr ();
    effect_renderer = EffectRendererPtr ();    
  }

  void UpdateCacheCore ()
  {
    try
    {
      cached_effect = effect.Resource ();      
      
      if (!cached_effect)
        return;

      effect_renderer = EffectRendererPtr (new render::manager::EffectRenderer (cached_effect, device_manager, frame, &properties_layout), false);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::EffectHolder::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
  using CacheHolder::InvalidateCache;
};

/*
    Описание объекта рендеринга
*/

struct EntityDesc
{
  EntityPtr                  entity;          //объект
  void*                      user_data;       //пользовательские данные
  LowLevelBufferPtr          property_buffer; //буфер динамических свойств объекта
  ProgramParametersLayoutPtr layout;          //расположение динамических свойств объекта
  
  EntityDesc (const EntityPtr& in_entity, void* in_user_data)
    : entity (in_entity)
    , user_data (in_user_data)
  {
  }
};

}

/*
    Описание реализации кадра
*/

typedef stl::vector<FramePtr>   FrameArray;
typedef stl::vector<EntityDesc> EntityArray;

struct FrameImpl::Impl: public CacheHolder
{
  stl::auto_ptr<EffectHolder>          effect_holder;           //хранилище эффекта
  EntityArray                          entities;                //список объектов, отображаемых в кадре
  FrameArray                           frames;                  //список вложенных кадров
  PropertyBuffer                       properties;              //свойства кадра
  PropertyCachePtr                     entities_properties;     //динамические свойства объектов
  render::manager::ShaderOptionsCache  shader_options_cache;    //кэш опций шейдера
  RenderTargetMapPtr                   render_targets;          //целевые буферы отрисовки
  TextureMap                           textures;                //локальные текстуры фрейма
  bool                                 scissor_state;           //включено ли отсечение
  size_t                               clear_flags;             //флаги очистки
  math::vec4f                          clear_color;             //цвет очистки фона
  float                                clear_depth_value;       //значение буфера глубины после очистки
  unsigned char                        clear_stencil_index;     //значение буфера трафарета после очистки
  LowLevelBufferPtr                    cached_properties;       //закэшированные свойства
  EntityDrawFunction                   entity_draw_handler;     //обработчик рисования объектов
  EntityDrawParams                     entity_draw_params;      //параметры рисования объектов (кэш)
  bool                                 auto_cleanup_state;      //самоочистка кадра после отрисовки

///Конструктор
  Impl (FrameImpl& owner, const DeviceManagerPtr& device_manager, const EffectManagerPtr& effect_manager, const PropertyCachePtr& in_property_cache)
    : effect_holder (new EffectHolder (effect_manager, device_manager, owner))
    , properties (device_manager)
    , entities_properties (in_property_cache)
    , shader_options_cache (&device_manager->CacheManager ())
    , render_targets (new RenderTargetMapImpl, false)
    , scissor_state (false)
    , clear_flags (ClearFlag_All)
    , clear_depth_value (1.0f)
    , clear_stencil_index (0)
    , auto_cleanup_state (true)
  {
    AttachCacheSource (*effect_holder);
    AttachCacheSource (properties);
    
    effect_holder->properties_layout.AttachSlot (ProgramParametersSlot_Frame, properties.Properties ());
    
    entities.reserve (RESERVED_ENTITIES_COUNT);
    frames.reserve (RESERVED_FRAMES_COUNT);
  }
  
///Получение номера уровня детализации по расстоянию
  size_t GetLod (double distance)
  {
    return size_t (log (distance));
  }
  
///Работа с кэшем
  void ResetCacheCore ()
  {
    cached_properties = LowLevelBufferPtr ();
  }

  void UpdateCacheCore ()
  {
    try
    {
      effect_holder->UpdateCache ();

      cached_properties = properties.Buffer ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::FrameImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

/*
   Конструктор / деструктор
*/

FrameImpl::FrameImpl (const DeviceManagerPtr& device_manager, const EffectManagerPtr& effect_manager, const PropertyCachePtr& property_cache)
{
  try
  {
    if (!effect_manager)
      throw xtl::make_null_argument_exception ("", "effect_manager");
    
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
      
    if (!property_cache)
      throw xtl::make_null_argument_exception ("", "property_cache");
    
    impl = new Impl (*this, device_manager, effect_manager, property_cache);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::FrameImpl");
    throw;
  }
}

FrameImpl::~FrameImpl ()
{
}

/*
    Карта целей рендеринга
*/

void FrameImpl::SetRenderTargets (RenderTargetMapImpl& map)
{
  impl->render_targets = &map;
}

RenderTargetMapImpl& FrameImpl::RenderTargets ()
{
  return *impl->render_targets;
}

/*
    Область отсечения
*/

void FrameImpl::SetScissorState (bool state)
{
  impl->scissor_state = state;
}

bool FrameImpl::ScissorState ()
{
  return impl->scissor_state;
}

/*
    Флаги очистки кадра
*/

void FrameImpl::SetClearFlags (size_t clear_flags)
{
  impl->clear_flags = clear_flags;
}

size_t FrameImpl::ClearFlags ()
{
  return impl->clear_flags;
}

/*
    Параметры очистки буфера цвета
*/

void FrameImpl::SetClearColor (const math::vec4f& color)
{
  impl->clear_color = color;
}

const math::vec4f& FrameImpl::ClearColor ()
{
  return impl->clear_color;
}

/*
    Параметры очистки буфера попиксельного отсечения
*/

void FrameImpl::SetClearDepthValue (float depth_value)
{
  impl->clear_depth_value = depth_value;
}

void FrameImpl::SetClearStencilIndex (unsigned char stencil_index)
{
  impl->clear_stencil_index = stencil_index;
}

float FrameImpl::ClearDepthValue ()
{
  return impl->clear_depth_value;
}

unsigned char FrameImpl::ClearStencilIndex ()
{
  return impl->clear_stencil_index;
}

/*
    Локальные текстуры
*/

void FrameImpl::SetLocalTexture (const char* name, const TexturePtr& texture)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!texture)
      throw xtl::make_null_argument_exception ("", "texture");
      
    TextureMap::iterator iter = impl->textures.find (name);
    
    if (iter != impl->textures.end ())
      throw xtl::make_argument_exception ("", "name", name, "Frame local texture has already registered");
      
    impl->textures.insert_pair (name, texture);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::SetLocalTexture");
    throw;
  }
}

void FrameImpl::RemoveLocalTexture (const char* name)
{
  if (!name)
    return;
    
  impl->textures.erase (name);
}

void FrameImpl::RemoveAllLocalTextures ()
{
  impl->textures.clear ();
}

/*
    Получение локальной текстуры
*/

TexturePtr FrameImpl::FindLocalTexture (const char* name)
{
  if (!name)
    return TexturePtr ();
    
  TextureMap::iterator iter = impl->textures.find (name);
  
  if (iter == impl->textures.end ())
    return TexturePtr ();
    
  return iter->second;
}

TexturePtr FrameImpl::LocalTexture (const char* name)
{
  static const char* METHOD_NAME = "render::manager::FrameImpl::LocalTexture";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  TexturePtr texture = FindLocalTexture (name);
  
  if (texture)
    return texture;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Local texture not found");

}

/*
    Установка эффекта рендеринга
*/

void FrameImpl::SetEffect (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!strcmp (impl->effect_holder->effect.Name (), name))
      return;

    impl->effect_holder->effect = impl->effect_holder->effect_manager->GetEffectProxy (name);    
    
    impl->effect_holder->InvalidateCache ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::SetEffect");
    throw;
  }
}

const char* FrameImpl::Effect ()
{
  return impl->effect_holder->effect.Name ();
}

/*
    Рендер эффекта
*/

EffectRendererPtr FrameImpl::EffectRenderer ()
{
  try
  {
    impl->UpdateCache ();
    
    return impl->effect_holder->effect_renderer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::EffectRenderer");
    throw;
  }
}

/*
    Свойства рендеринга
*/

void FrameImpl::SetProperties (const common::PropertyMap& properties)
{
  try
  {
    impl->properties.SetProperties (properties);

    impl->effect_holder->properties_layout.AttachSlot (ProgramParametersSlot_Frame, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::SetProperties");
    throw;
  }
}

const common::PropertyMap& FrameImpl::Properties ()
{
  return impl->properties.Properties ();
}

/*
    Получение буфера свойств
*/

const LowLevelBufferPtr& FrameImpl::DevicePropertyBuffer ()
{
  try
  {
    impl->UpdateCache ();
    
    return impl->cached_properties;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::DevicePropertyBuffer");
    throw;
  }
}

/*
    Макро-определения шейдера
*/

void FrameImpl::SetShaderOptions (const common::PropertyMap& properties)
{
  try
  {
    impl->shader_options_cache.SetProperties (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::SetShaderOptions");
    throw;
  }
}

const common::PropertyMap& FrameImpl::ShaderOptions ()
{
  return impl->shader_options_cache.Properties ();
}

ShaderOptionsCache& FrameImpl::ShaderOptionsCache ()
{
  return impl->shader_options_cache;
}

/*
    Список отрисовки
*/

size_t FrameImpl::EntitiesCount ()
{
  return impl->entities.size ();
}

void FrameImpl::AddEntity (const EntityPtr& entity, void* user_data)
{
  impl->entities.push_back (EntityDesc (entity, user_data));
}

void FrameImpl::RemoveAllEntities ()
{
  impl->entities.clear ();
}

/*
    Добавление вложенных кадров
*/

size_t FrameImpl::FramesCount ()
{
  return impl->frames.size ();
}

void FrameImpl::AddFrame (const FramePtr& frame)
{
  impl->frames.push_back (frame);  
}

void FrameImpl::RemoveAllFrames ()
{
  impl->frames.clear ();  
}

/*
    Автоматическая очистка кадра после отрисовки
*/

void FrameImpl::SetAutoCleanup (bool state)
{
  impl->auto_cleanup_state = state;
}

bool FrameImpl::IsAutoCleanupEnabled ()
{
  return impl->auto_cleanup_state;
}

/*
    Установка пользовательского обработчика рисования объектов
*/

void FrameImpl::SetEntityDrawHandler (const EntityDrawFunction& handler)
{
  impl->entity_draw_handler = handler;
}

const Frame::EntityDrawFunction& FrameImpl::EntityDrawHandler ()
{
  return impl->entity_draw_handler;
}

/*
    Установка начальных свойств пары frame-entity
*/

void FrameImpl::SetInitialEntityDrawProperties (const common::PropertyMap& properties)
{
  impl->entity_draw_params.properties = properties;
}

const common::PropertyMap& FrameImpl::InitialEntityDrawProperties ()
{
  return impl->entity_draw_params.properties;
}

/*
    Подготовка к отрисовке кадра
*/

void FrameImpl::Prerender (EntityDrawFunction entity_draw_handler)
{
    //построение списка операций
    
  render::manager::EffectRenderer& renderer                = *impl->effect_holder->effect_renderer;    
  Frame                            frame                   = Wrappers::Wrap<Frame> (this);
  bool                             has_entity_draw_handler = entity_draw_handler;
  EntityDrawParams&                entity_draw_params      = impl->entity_draw_params;
  PropertyCache&                   entities_properties     = *impl->entities_properties;
  
  renderer.RemoveAllOperations ();
  
  entity_draw_params.mvp_matrix = math::mat4f (1.0f);

  for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
  {
    EntityDesc& desc = *iter;
    
      //пропуск рисования пустых объектов
    
    if (!desc.entity->LodsCount ())
      continue;
      
      //обновление динамических текстур
      
    DynamicTextureEntityStorage& dynamic_textures = desc.entity->DynamicTextureStorage ();
    
    dynamic_textures.Update (this);
      
      //вызов обработчика      

    size_t eye_distance = 0, lod = 0;
      
    if (has_entity_draw_handler)
    {
      Entity entity_wrap = Wrappers::Wrap<Entity> (desc.entity);

      entity_draw_handler (frame, entity_wrap, desc.user_data, entity_draw_params);
            
        //расчёт расстояния от z-near до объекта        
        
      math::vec4f mvp_lod_point = entity_draw_params.mvp_matrix * math::vec4f (desc.entity->LodPoint (), 1.0f); 
      double      distance      = stl::min (stl::max (mvp_lod_point.z / mvp_lod_point.w, 1.0f), 0.0f) * ~0u;
      
      eye_distance = size_t (distance);
      lod          = impl->GetLod (distance);
    }    

      //получение информации об уровне детализации

    const EntityLodDesc& lod_desc = desc.entity->GetLod (lod, true); 

      //заполнение константного буфера соответствующего паре frame-entity

    if (has_entity_draw_handler && lod_desc.has_frame_independent_operations)
    {
      entities_properties.Convert (entity_draw_params.properties, desc.property_buffer, desc.layout);
    }
    else
    {
      desc.property_buffer = LowLevelBufferPtr ();
      desc.layout          = ProgramParametersLayoutPtr ();
    }

      //добавление операций рендеринга

    renderer.AddOperations (lod_desc.operations, eye_distance, entity_draw_params.mvp_matrix, desc.property_buffer.get (), desc.layout.get ());
  }

  for (FrameArray::iterator iter=impl->frames.begin (), end=impl->frames.end (); iter!=end; ++iter)
  {
    renderer.AddOperations (**iter);
  }
}

/*
    Рисование кадра
*/

void FrameImpl::Draw (RenderingContext* previous_context)
{
  try
  {
      //обновление кэша

    UpdateCache ();

      //обновление маркеров отрисовки для корневых кадров
    
    if (!previous_context)
      impl->effect_holder->device_manager->CacheManager ().UpdateMarkers ();

    try
    {
        //формирование контекста отрисовки
      
      RenderingContext context (*this, previous_context);
      
      if (!impl->effect_holder->effect_renderer)
        throw xtl::format_operation_exception ("", "No effect assigned");
        
        //построение списка операций
        
      Prerender (impl->entity_draw_handler);
        
        //выполнение операций рендеринга

      impl->effect_holder->effect_renderer->ExecuteOperations (context);

        //самоочистка кадра

      if (impl->auto_cleanup_state)
      {
        RemoveAllFrames ();
        RemoveAllEntities ();
      }

        //очистка временных данных
      
      if (!previous_context)
        Cleanup ();
    }
    catch (...)
    {
      if (!previous_context)
        Cleanup ();      

      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::FrameImpl::Draw");
    throw;
  }
}

/*
    Очистка после рисования кадра
*/

void FrameImpl::Cleanup ()
{
  impl->entities_properties->Reset ();
  impl->effect_holder->device_manager->CacheManager ().FlushCaches ();    
}

/*
    Управление кэшированием
*/

void FrameImpl::UpdateCache ()
{
  impl->UpdateCache ();

  for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
    iter->entity->UpdateCache ();
}

void FrameImpl::ResetCache ()
{
  impl->ResetCache ();
  
  for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
    iter->entity->ResetCache ();  
}
