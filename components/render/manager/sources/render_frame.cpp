#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: update entity dynamic textures
//TODO: detect entity LOD

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

typedef stl::hash_map<stl::hash_key<const char*>, RenderTargetDescPtr> RenderTargetDescMap;
typedef stl::hash_map<stl::hash_key<const char*>, TexturePtr>          TextureMap;

/*
    Хранилище для эффекта
*/

struct EffectHolder: public CacheSource
{
  DeviceManagerPtr         device_manager;         //менеджер устройства отрисовки
  EffectManagerPtr         effect_manager;         //ссылка на менеджер эффектов
  EffectProxy              effect;                 //эффект кадра
  EffectRendererPtr        effect_renderer;        //рендер эффекта
  EffectPtr                cached_effect;          //закэшированный эффект
  ProgramParametersLayout  properties_layout;      //расположение свойств
  bool                     need_operations_update; //нужно ли обновлять список операций рендеринга
  
///Конструктор
  EffectHolder (const EffectManagerPtr& in_effect_manager, const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , effect_manager (in_effect_manager)
    , effect (effect_manager->GetEffectProxy (""))
    , properties_layout (&device_manager->Device ())
    , need_operations_update (true)
  {
    effect.AttachCacheHolder (*this);
  }
  
///Работа с кэшем
  void ResetCacheCore ()
  {
    cached_effect   = EffectPtr ();
    effect_renderer = EffectRendererPtr ();
    
    need_operations_update = true;
  }

  void UpdateCacheCore ()
  {
    try
    {
      cached_effect = effect.Resource ();
      
      if (!cached_effect)
        return;
        
      effect_renderer = EffectRendererPtr (new render::EffectRenderer (cached_effect, device_manager, &properties_layout), false);
      
      need_operations_update = true;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::EffectHolder::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

/*
    Описание объекта рендеринга
*/

struct EntityDesc
{
  EntityPtr                  entity;          //объект
  LowLevelBufferPtr          property_buffer; //буфер динамических свойств объекта
  ProgramParametersLayoutPtr layout;          //расположение динамических свойств объекта
  
  EntityDesc (const EntityPtr& in_entity, const LowLevelBufferPtr& in_buffer = LowLevelBufferPtr (), ProgramParametersLayoutPtr in_layout = ProgramParametersLayoutPtr ())
    : entity (in_entity)
    , property_buffer (in_buffer)
    , layout (in_layout)
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
  stl::auto_ptr<EffectHolder> effect_holder;          //хранилище эффекта
  EntityArray                 entities;               //список объектов, отображаемых в кадре
  FrameArray                  frames;                 //список вложенных кадров
  PropertyBuffer              properties;             //свойства кадра
  PropertyCache               entities_properties;    //динамические свойства объектов
  RenderTargetDescMap         render_targets;         //целевые буферы отрисовки
  TextureMap                  textures;               //локальные текстуры фрейма
  bool                        scissor_state;          //включено ли отсечение
  size_t                      clear_flags;            //флаги очистки
  math::vec4f                 clear_color;            //цвет очистки фона
  float                       clear_depth_value;      //значение буфера глубины после очистки
  unsigned char               clear_stencil_index;    //значение буфера трафарета после очистки
  LowLevelBufferPtr           cached_properties;      //закэшированные свойства

///Конструктор
  Impl (const DeviceManagerPtr& device_manager, const EffectManagerPtr& effect_manager)
    : effect_holder (new EffectHolder (effect_manager, device_manager))
    , properties (device_manager)
    , entities_properties (device_manager)
    , scissor_state (false)
    , clear_flags (ClearFlag_All)
    , clear_depth_value (1.0f)
    , clear_stencil_index (0)
  {
    AttachCacheSource (*effect_holder);
    AttachCacheSource (properties);
    
    entities.reserve (RESERVED_ENTITIES_COUNT);
    frames.reserve (RESERVED_FRAMES_COUNT);
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
      cached_properties = properties.Buffer ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::FrameImpl::Impl::UpdateCacheCore");
      throw;
    }
  }
    
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

/*
   Конструктор / деструктор
*/

FrameImpl::FrameImpl (const DeviceManagerPtr& device_manager, const EffectManagerPtr& effect_manager)
{
  try
  {
    if (!effect_manager)
      throw xtl::make_null_argument_exception ("", "effect_manager");
    
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");
    
    impl = new Impl (device_manager, effect_manager);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::FrameImpl");
    throw;
  }
}

FrameImpl::~FrameImpl ()
{
}

/*
    Регистрация целевых буферов отрисовки
*/

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target)
{
  SetRenderTarget (name, target, RectAreaPtr (), RectAreaPtr ());
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport)
{
  SetRenderTarget (name, target, viewport, RectAreaPtr ());
}

void FrameImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const RectAreaPtr& viewport, const RectAreaPtr& scissor)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!target)
      throw xtl::make_null_argument_exception ("", "target");

    RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

    if (iter != impl->render_targets.end ())
      throw xtl::make_argument_exception ("", "name", name, "Render target has already registered");

    impl->render_targets.insert_pair (name, RenderTargetDescPtr (new render::RenderTargetDesc (target, viewport, scissor), false));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetRenderTarget(const char*,const RenderTargetPtr&,const RectAreaPtr&,const RectAreaPtr&)");
    throw;
  }
}

void FrameImpl::RemoveRenderTarget (const char* name)
{
  if (!name)
    return;
    
  impl->render_targets.erase (name);
}

void FrameImpl::RemoveAllRenderTargets ()
{
  impl->render_targets.clear ();
}

/*
    Получение целевых буферов отрисовки
*/

RenderTargetPtr FrameImpl::FindRenderTarget (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (!desc)
    return RenderTargetPtr ();
  
  return desc->render_target;
}

RectAreaPtr FrameImpl::FindViewport (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (!desc)
    return RectAreaPtr ();
    
  return desc->viewport;
}

RectAreaPtr FrameImpl::FindScissor (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);

  if (!desc)
    return RectAreaPtr ();

  return desc->scissor;
}

RenderTargetDescPtr FrameImpl::FindRenderTargetDesc (const char* name)
{
  if (!name)
    return RenderTargetDescPtr ();

  RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

  if (iter == impl->render_targets.end ())
    return RenderTargetDescPtr ();
    
  return iter->second;
}

RenderTargetPtr FrameImpl::RenderTarget (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::RenderTarget";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetPtr target = FindRenderTarget (name);
  
  if (target)
    return target;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
}

RectAreaPtr FrameImpl::Viewport (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::Viewport";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindViewport (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Viewport not found");
}

RectAreaPtr FrameImpl::Scissor (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::Scissor";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindScissor (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Scissor not found");
}

RenderTargetDescPtr FrameImpl::RenderTargetDesc (const char* name)
{
  static const char* METHOD_NAME = "render::FrameImpl::RenderTargetDesc";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (desc)
    return desc;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
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
    e.touch ("render::FrameImpl::SetLocalTexture");
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
  static const char* METHOD_NAME = "render::FrameImpl::LocalTexture";
  
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
    
    impl->effect_holder->effect = impl->effect_holder->effect_manager->GetEffectProxy (name);
    
    impl->effect_holder->need_operations_update = true;
    
    impl->effect_holder->ResetCache ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetEffect");
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
    e.touch ("render::FrameImpl::EffectRenderer");
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

    if (impl->effect_holder)
      impl->effect_holder->properties_layout.AttachSlot (ProgramParametersSlot_Frame, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::SetProperties");
    throw;
  }
}

const common::PropertyMap& FrameImpl::Properties ()
{
  return impl->properties.Properties ();
}

/*
    Список отрисовки
*/

size_t FrameImpl::EntitiesCount ()
{
  return impl->entities.size ();
}

void FrameImpl::AddEntity (const EntityPtr& entity)
{
  impl->entities.push_back (EntityDesc (entity));
  
  impl->effect_holder->need_operations_update = true;
}

void FrameImpl::AddEntity (const EntityPtr& entity, const common::PropertyMap& properties)
{
  try
  {
    LowLevelBufferPtr          properties_buffer = 0;
    ProgramParametersLayoutPtr properties_layout = 0;
    
    impl->entities_properties.Convert (properties, properties_buffer, properties_layout);
    
    impl->entities.push_back (EntityDesc (entity, properties_buffer, properties_layout));
  
    impl->effect_holder->need_operations_update = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::AddEntity(const EntityPtr&,const common::PropertyMap&)");
    throw;
  }
}

void FrameImpl::RemoveAllEntities ()
{
  impl->entities.clear ();
  impl->entities_properties.Flush ();
  
  impl->effect_holder->need_operations_update = true;
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
  
  impl->effect_holder->need_operations_update = true;
}

void FrameImpl::RemoveAllFrames ()
{
  impl->frames.clear ();
  
  impl->effect_holder->need_operations_update = true;
}

/*
    Рисование кадра
*/

void FrameImpl::Draw (RenderingContext* previous_context)
{
  try
  {
    RenderingContext context (*this, previous_context);
    
    if (!impl->effect_holder->effect_renderer)
      throw xtl::format_operation_exception ("", "No effect assigned");            
      
      //построение списка операций
      
    if (impl->effect_holder->need_operations_update)
    {
      render::EffectRenderer& renderer = *impl->effect_holder->effect_renderer;
      
      renderer.RemoveAllOperations ();
      
      for (EntityArray::iterator iter=impl->entities.begin (), end=impl->entities.end (); iter!=end; ++iter)
      {
        EntityDesc& desc = *iter;
        
        if (!desc.entity->LodsCount ())
          continue;
          
        size_t lod = 0;

        renderer.AddOperations (desc.entity->RendererOperations (lod, true), desc.property_buffer.get (), desc.layout.get ());
      }
      
      for (FrameArray::iterator iter=impl->frames.begin (), end=impl->frames.end (); iter!=end; ++iter)
      {
        renderer.AddOperations (**iter);
      }
    }      

      //выполнение операций рендеринга

    impl->effect_holder->effect_renderer->ExecuteOperations (context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::FrameImpl::Draw");
    throw;
  }
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
