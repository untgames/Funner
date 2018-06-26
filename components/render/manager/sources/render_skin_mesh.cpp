#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    EntityJointList
===================================================================================================
*/

/*
    Конструктор
*/

EntityJointList::EntityJointList ()
  : update_revision_id ()
{
}

/*
    Количество соединений
*/

void EntityJointList::Resize (size_t joints_count)
{
  transformations.resize (joints_count, math::mat4f (1.0f));

  update_revision_id++;
}

/*
    Трансформации
*/

const math::mat4f* EntityJointList::Transformations ()
{
  if (transformations.empty ())
    return 0;

  return &transformations [0];
}

void EntityJointList::SetTransformation (size_t joint_index, const math::mat4f& tm)
{
  if (joint_index >= transformations.size ())
    throw xtl::make_range_exception ("render::manager::EntityJointList::SetTransformation", "joint_index", joint_index, transformations.size ());

  transformations [joint_index] = tm;
  update_revision_id++;
}

const math::mat4f& EntityJointList::Transformation (size_t joint_index)
{
  if (joint_index >= transformations.size ())
    throw xtl::make_range_exception ("render::manager::EntityJointList::Transformation", "joint_index", joint_index, transformations.size ());

  return transformations [joint_index];
}

/*
===================================================================================================
    SkinVertexBuffer
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

SkinVertexBuffer::SkinVertexBuffer (const DeviceManagerPtr& device_manager, const VertexBufferPtr& in_vertex_buffer)
  : vertex_buffer (in_vertex_buffer)
  , cached_update_revision_id (~0u)
{
  try
  {
    if (!vertex_buffer)
      throw xtl::make_null_argument_exception ("", "vertex_buffer");

    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");

    low_level::IDevice& device = device_manager->Device ();

    vertices_count       = vertex_buffer->VerticesCount ();
    vertex_cache         = vertex_buffer->SkinVertices ();
    vertex_weights       = vertex_buffer->VertexWeights ();
    vertex_stream_index  = vertex_buffer->SkinStreamIndex ();

    if (!vertex_weights || !vertex_cache || vertex_stream_index == -1)
      throw xtl::format_operation_exception ("", "Invalid skin data in vertex buffer");

    low_level::BufferDesc desc;

    memset (&desc, 0, sizeof desc);

    desc.size         = sizeof (SkinVertex) * vertices_count;
    desc.bind_flags   = low_level::BindFlag_VertexBuffer;
    desc.access_flags = low_level::AccessFlag_Write;
    desc.usage_mode   = low_level::UsageMode_Dynamic;
        
    vertex_stream = LowLevelBufferPtr (device.CreateBuffer (desc), false);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::SkinVertexBuffer::SkinVertexBuffer");
    throw;
  }
}

/*
    Обновление буфера
*/

void SkinVertexBuffer::Update (EntityJointList& joints)
{
  if (cached_update_revision_id == joints.UpdateRevisionId ())
    return;

    //расчет вершин скин меша

  size_t              joints_count     = joints.Size ();
  const math::mat4f*  transformations  = joints.Transformations ();
  const VertexWeight* src_vertex       = vertex_weights;
  SkinVertex*         dst_vertex       = vertex_cache;

  static math::mat4f default_tm (1.0f);

  for (size_t i=0; i<vertices_count; i++, src_vertex++, dst_vertex++)
  {
    math::vec3f dst_position, dst_normal;

    const math::vec3f        &src_position = src_vertex->position,
                             &src_normal   = src_vertex->normal;
    const VertexJointWeight* src_weight    = src_vertex->first_weight;

    for (size_t j=0, count=src_vertex->weights_count; j<count; j++, src_weight++)
    {
      size_t             joint_index = src_weight->joint_index;
      float              weight      = src_weight->weight;
      const math::mat4f& src_tm      = joint_index < joints_count ? transformations [joint_index] : default_tm;

      dst_position += (src_tm * src_position) * weight;
      dst_normal   += (src_tm * src_normal) * weight;
    }

    dst_vertex->position = dst_position;
    dst_vertex->normal   = dst_normal;
  }

    //обновление данных

  vertex_stream->SetData (0, sizeof (SkinVertex) * vertices_count, vertex_cache);

    //обновление кэш метки

  cached_update_revision_id = joints.UpdateRevisionId ();
}

/*
===================================================================================================
    SkinDynamicPrimitive
===================================================================================================
*/

/*
    Конструктор
*/

SkinDynamicPrimitive::SkinDynamicPrimitive
 (const SkinVertexBufferPtr&          in_skin_vertex_buffer,
  EntityImpl&                         in_entity,
  const FillRendererPrimitiveHandler& fill_renderer_primitive_fn)
try
  : DynamicPrimitive (static_cast<manager::RendererPrimitiveGroup*> (0), DynamicPrimitiveFlag_EntityDependent)
  , entity (in_entity)
  , skin_vertex_buffer (in_skin_vertex_buffer)
  , fill_renderer_primitive_handler(fill_renderer_primitive_fn)
  , cached_state_block_mask_hash (0)
{
}
catch (xtl::exception& e)
{
  e.touch ("render::manager::SkinDynamicPrimitive::SkinDynamicPrimitive");
  throw;
}

SkinDynamicPrimitive::~SkinDynamicPrimitive ()
{
  SkinDynamicPrimitive::ResetCacheCore ();
}


/*
    Обновление кэша
*/

void SkinDynamicPrimitive::UpdateCacheCore ()
{
  try
  {
    bool has_debug_log = entity.DeviceManager ()->Settings ().HasDebugLog ();

    if (has_debug_log)
      log.Printf ("Update skin dynamic primitive (id=%u)", Id ());

      //получение данных примитива

    memset (&cached_primitive, 0, sizeof cached_primitive);

    fill_renderer_primitive_handler (cached_primitive);

    if (!cached_primitive.state_block)
    {
      SkinDynamicPrimitive::ResetCacheCore ();
      return;
    }

      //получение вершинного буфера

    int                      vertex_buffer_index = skin_vertex_buffer->VertexStreamIndex ();
    const LowLevelBufferPtr& vertex_buffer       = skin_vertex_buffer->VertexStream ();

    if (!vertex_buffer || vertex_buffer_index < 0 || vertex_buffer_index >= low_level::DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
    {
      SkinDynamicPrimitive::ResetCacheCore ();
      return;
    }

      //обновление блока состояний

    render::low_level::IDevice&        device        = entity.DeviceManager ()->Device ();
    render::low_level::IDeviceContext& context       = entity.DeviceManager ()->ImmediateContext ();

    low_level::StateBlockMask mask;
    size_t                    mask_hash = mask.Hash ();
      
    if (cached_state_block_mask_hash != mask_hash || !cached_state_block)
    {
      cached_primitive.state_block->GetMask (mask);

      mask.is_vertex_buffers [vertex_buffer_index] = true;

      if (has_debug_log)
        log.Printf ("...create state block for skin dynamic mesh primitive");
        
      cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
      cached_state_block_mask_hash = mask_hash;
    }

    cached_state_block->Apply (&context);

    context.ISSetVertexBuffer (vertex_buffer_index, vertex_buffer.get ());

    cached_state_block->Capture (&context);

      //обновление примитива
  
    cached_primitive.state_block = cached_state_block.get ();

      //обновление группы примитивов

    manager::RendererPrimitiveGroup& group = *this;

    group.primitives_count = 1;
    group.primitives       = &cached_primitive;

      //обновление зависимостей всегда, поскольку любые изменения материала/примитива должны быть отображены на зависимые кэши
        
    InvalidateCacheDependencies ();
      
    if (has_debug_log)
      log.Printf ("...skin dynamic mesh cache updated");
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::SkinDynamicPrimitive::UpdateCacheCore");
    SkinDynamicPrimitive::ResetCacheCore ();
    throw;
  }
}

void SkinDynamicPrimitive::ResetCacheCore ()
{
  if (entity.DeviceManager ()->Settings ().HasDebugLog ())
    log.Printf ("Reset skin dynamic primtive (id=%u)", Id ());        
 
  memset (&cached_primitive, 0, sizeof cached_primitive);

  manager::RendererPrimitiveGroup& group = *this;

  group.primitives_count = 0;
  group.primitives       = 0;

  cached_state_block = LowLevelStateBlockPtr ();

  cached_state_block_mask_hash = 0;
}

/*
    Обновление
*/

void SkinDynamicPrimitive::UpdateOnPrerenderCore (EntityImpl& in_entity)
{
  try
  {
      //проверка корректности аргументов

    if (&in_entity != &entity)
      throw xtl::format_operation_exception ("", "Entity for SkinDynamicPrimitive::UpdateOnPrerenderCore should be same as in constructor");

    EntityJointListPtr joints = entity.Joints ();

    if (!joints)
      return;

      //обновление вершинного буфера

    skin_vertex_buffer->Update (*joints);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::SkinDynamicPrimitive::UpdateOnPrerenderCore");
    throw;
  }
}

/*
    SkinDynamicPrimitivePrototype
*/

///Конструктор
SkinDynamicPrimitivePrototype::SkinDynamicPrimitivePrototype (render::manager::VertexBuffer& in_vertex_buffer, const FillRendererPrimitiveHandler& fill_renderer_primitive_fn)
  : vertex_buffer (in_vertex_buffer)
  , fill_renderer_primitive_handler (fill_renderer_primitive_fn)
{
}

///Создание экземпляра
DynamicPrimitive* SkinDynamicPrimitivePrototype::CreateDynamicPrimitiveInstance (EntityImpl& entity, DynamicPrimitiveEntityStorage& dynamic_primitive_storage)
{
  try
  {
    SkinVertexBufferPtr skin_vertex_buffer;

    if (SkinVertexBufferPtr* skin_vertex_buffer_ptr = dynamic_primitive_storage.FindCacheValue<SkinVertexBufferPtr> (&vertex_buffer, true))
    {
      skin_vertex_buffer = *skin_vertex_buffer_ptr;
    }
    else
    {
      skin_vertex_buffer = SkinVertexBufferPtr (new SkinVertexBuffer (entity.DeviceManager (), &vertex_buffer), false);

      dynamic_primitive_storage.SetCacheValue<SkinVertexBufferPtr> (skin_vertex_buffer, &vertex_buffer);
    }

    return new SkinDynamicPrimitive (skin_vertex_buffer, entity, fill_renderer_primitive_handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::SkinDynamicPrimitivePrototype::CreateDynamicPrimitiveInstance");
    throw;
  }
}

///Обновление кэш значений
void SkinDynamicPrimitivePrototype::TouchCacheValues (DynamicPrimitiveEntityStorage& storage)
{
  storage.FindCacheValue<SkinVertexBufferPtr> (&vertex_buffer, true);
}
