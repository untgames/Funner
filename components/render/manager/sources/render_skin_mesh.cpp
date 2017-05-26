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

SkinVertexBuffer::SkinVertexBuffer (low_level::IDevice& device, const VertexBufferPtr& in_vertex_buffer, const EntityJointListPtr& in_joints)
  : vertex_buffer (in_vertex_buffer)
  , joints (in_joints)
  , cached_update_revision_id (~0u)
{
  try
  {
    if (!vertex_buffer)
      throw xtl::make_null_argument_exception ("", "vertex_buffer");

    if (joints)
      throw xtl::make_null_argument_exception ("", "joints");

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

void SkinVertexBuffer::Update ()
{
  if (cached_update_revision_id == joints->UpdateRevisionId ())
    return;

    //расчет вершин скин меша

  size_t              joints_count     = joints->Size ();
  const math::mat4f*  transformations  = joints->Transformations ();
  const VertexWeight* src_vertex       = vertex_weights;
  SkinVertex*         dst_vertex       = vertex_cache;

  static math::mat4f default_tm (1.0f);

  for (size_t i=0; i<vertices_count; i++, src_vertex++, dst_vertex++)
  {
    math::vec4f dst_position, dst_normal;

    const math::vec4f        &src_position = src_vertex->position,
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

  cached_update_revision_id = joints->UpdateRevisionId ();
}
