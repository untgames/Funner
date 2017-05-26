#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
    Конструкторы / деструктор
*/

VertexBuffer::VertexBuffer (const media::geometry::VertexBuffer& source, PrimitiveBuffersImpl& buffers, const DeviceManagerPtr& device_manager, MeshBufferUsage usage)
  : vertices_count(source.VerticesCount ())
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");

    low_level::IDevice& device = device_manager->Device ();

    InputLayoutManager& layout_manager = device_manager->InputLayoutManager ();

      //резервирование вершинных атрибутов
    
    unsigned int attributes_count = 0;
    
    for (unsigned int i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
      attributes_count += source.Stream (i).Format ().AttributesCount ();

    attributes.reserve (attributes_count);

    streams.reserve (source.StreamsCount ());
    vertex_formats.reserve (source.StreamsCount ());

      //обработка случая skinning-mesh

    if (source.Weights ().Size ())
      ProcessVertexWeights (source);

    bool has_vertex_weights = VertexWeights () != 0;

      //конвертация

    for (unsigned int i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
    {
      const media::geometry::VertexStream& vs = source.Stream (i);
      
      LowLevelBufferPtr vs_buffer = buffers.CreateVertexStream (vs, usage);

      media::geometry::VertexFormat vertex_format = Clone (layout_manager, vs.Format ());      

      for (unsigned int j=0, attr_count=vertex_format.AttributesCount (); j<attr_count; j++)
      {
        const media::geometry::VertexAttribute& src_va = vertex_format.Attribute (j);
        VertexAttribute                         dst_va;

        memset (&dst_va, 0, sizeof (dst_va));

        if (*src_va.name)
        {
          dst_va.semantic = src_va.name; //can't be changed in future, stored in internal InputLayoutManager cache and in this object
        }
        else
        {
          switch (src_va.semantic)
          {
            case media::geometry::VertexAttributeSemantic_Position:
              if (has_vertex_weights)
                continue;

              dst_va.semantic = device.GetVertexAttributeSemanticName (VertexAttributeSemantic_Position);
              break;
            case media::geometry::VertexAttributeSemantic_Normal:
              if (has_vertex_weights)
                continue;

              dst_va.semantic = device.GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal);
              break;
            case media::geometry::VertexAttributeSemantic_Color:
              dst_va.semantic = device.GetVertexAttributeSemanticName (VertexAttributeSemantic_Color);
              break;
            case media::geometry::VertexAttributeSemantic_TexCoord0:
            case media::geometry::VertexAttributeSemantic_TexCoord1:
            case media::geometry::VertexAttributeSemantic_TexCoord2:
            case media::geometry::VertexAttributeSemantic_TexCoord3:
            case media::geometry::VertexAttributeSemantic_TexCoord4:
            case media::geometry::VertexAttributeSemantic_TexCoord5:
            case media::geometry::VertexAttributeSemantic_TexCoord6:
            case media::geometry::VertexAttributeSemantic_TexCoord7:          
              dst_va.semantic = device.GetVertexAttributeSemanticName ((VertexAttributeSemantic)(VertexAttributeSemantic_TexCoord0 + src_va.semantic - media::geometry::VertexAttributeSemantic_TexCoord0));
              break;
            default:
              continue;
          }
        }

        switch (src_va.type)
        {
          case media::geometry::VertexAttributeType_Float2:
            dst_va.type   = InputDataType_Float;
            dst_va.format = InputDataFormat_Vector2;
            break;
          case media::geometry::VertexAttributeType_Float3:
            dst_va.type   = InputDataType_Float;
            dst_va.format = InputDataFormat_Vector3;
            break;
          case media::geometry::VertexAttributeType_Float4:
            dst_va.type   = InputDataType_Float;
            dst_va.format = InputDataFormat_Vector4;
            break;
          case media::geometry::VertexAttributeType_Short2:
            dst_va.type   = InputDataType_ShortNorm;
            dst_va.format = InputDataFormat_Vector2;
            break;
          case media::geometry::VertexAttributeType_Short3:
            dst_va.type   = InputDataType_ShortNorm;
            dst_va.format = InputDataFormat_Vector3;
            break;
          case media::geometry::VertexAttributeType_Short4:
            dst_va.type   = InputDataType_ShortNorm;
            dst_va.format = InputDataFormat_Vector4;
            break;
          case media::geometry::VertexAttributeType_UByte4:
            dst_va.type   = InputDataType_UByteNorm;
            dst_va.format = InputDataFormat_Vector4;
            break;
          default:
            continue;
        }

        dst_va.stride = vs.VertexSize ();
        dst_va.slot   = i;
        dst_va.offset = src_va.offset;

        attributes.push_back (dst_va);
      }

      streams.push_back (vs_buffer);
    }

      //добавление скин-данных

    if (has_vertex_weights)
    {
      skin_data->stream_index = streams.size ();

      VertexAttribute va [2];

      memset (&va, 0, sizeof (va));

      va [0].semantic = device.GetVertexAttributeSemanticName (VertexAttributeSemantic_Position);
      va [0].type     = InputDataType_Float;
      va [0].format   = InputDataFormat_Vector4;
      va [0].stride   = sizeof (SkinVertex);
      va [0].slot     = skin_data->stream_index;
      va [0].offset   = 0;

      va [1].semantic = device.GetVertexAttributeSemanticName (VertexAttributeSemantic_Normal);
      va [1].type     = InputDataType_Float;
      va [1].format   = InputDataFormat_Vector4;
      va [1].stride   = sizeof (SkinVertex);
      va [1].slot     = skin_data->stream_index;
      va [1].offset   = sizeof (math::vec4f);

      attributes.push_back (va [0]);
      attributes.push_back (va [1]);
    }

      //расчет хэша

    attributes_hash = InputLayoutManager::GetVertexAttributesHash (attributes.size (), &attributes [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::VertexBuffer::VertexBuffer");
    throw;
  }
}

/*
    Клонирование вершинного формата
*/

media::geometry::VertexFormat VertexBuffer::Clone (InputLayoutManager& manager, const media::geometry::VertexFormat& format)
{
  try
  {
    media::geometry::VertexFormat clone = manager.Clone (format);

    vertex_formats.push_back (clone);

    return clone;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::VertexBuffer::Clone");
    throw;
  }
}

/*
    Вершинные потоки
*/

unsigned int VertexBuffer::StreamsCount ()
{
  return (unsigned int)streams.size ();
}

const LowLevelBufferPtr* VertexBuffer::Streams ()
{
  if (streams.empty ())
    return 0;
    
  return &streams [0];
}

/*
    Вершинные веса
*/

const VertexWeight* VertexBuffer::VertexWeights ()
{
  if (!skin_data)
    return 0;

  return &skin_data->vertex_weights [0];
}

/*
    Временный буфер для расчета скин-вершин / номер слота скин-данных
*/

SkinVertex* VertexBuffer::SkinVertices ()
{
  if (!skin_data)
    return 0;

  return &skin_data->skin_vertices [0];
}

int VertexBuffer::SkinStreamIndex ()
{
  if (!skin_data)
    return -1;

  return skin_data->stream_index;
}

/*
    Получение лэйаута
*/

LowLevelInputLayoutPtr VertexBuffer::CreateInputLayout (InputLayoutManager& layout_manager, InputDataType type)
{
  try
  {
    if (type < 0 || type >= InputDataType_Num)
      throw xtl::make_argument_exception ("", "type", type);
      
      //проверка наличия лэйаута в кэше
      
    LowLevelInputLayoutPtr& layout = layouts [type];
      
    if (layout)
      return layout;
      
      //создание нового лэйаута
      
    InputLayoutDesc layout_desc;

    memset (&layout_desc, 0, sizeof layout_desc);

    layout_desc.vertex_attributes_count = (unsigned int)attributes.size ();
    layout_desc.vertex_attributes       = &attributes [0];
    layout_desc.index_type              = type;
    layout_desc.index_buffer_offset     = 0;

    layout = layout_manager.CreateInputLayout (attributes_hash, layout_desc);
      
    return layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::VertexBuffer::CreateInputLayout");
    throw;
  }
}

/*
    Обработка вершинных весов
*/

void VertexBuffer::ProcessVertexWeights (const media::geometry::VertexBuffer& source)
{
  SkinData tmp_skin_data;

    //определение исходных данных

  const media::geometry::VertexWeight* src_vertex_weights = source.Weights ().Data ();
  size_t                               src_vertex_weights_count = source.Weights ().Size ();

  const char *positions = 0, *normals = 0, *vertex_influences = 0;
  size_t position_stride = 0, normal_stride = 0, vertex_influence_stride = 0;

  for (unsigned int i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
  {
    const media::geometry::VertexStream& vs = source.Stream (i);
    const media::geometry::VertexFormat& vertex_format = vs.Format ();
    const void*                          data = vs.Data ();
    size_t                               vertex_size = vs.VertexSize ();

    for (unsigned int j=0, attr_count=vertex_format.AttributesCount (); j<attr_count; j++)
    {
      const media::geometry::VertexAttribute& va = vertex_format.Attribute (j);

      switch (va.semantic)
      {
        case media::geometry::VertexAttributeSemantic_Position:
          switch (va.type)
          {
            case media::geometry::VertexAttributeType_Float3:
            case media::geometry::VertexAttributeType_Float4:
              break;
            default:
              continue;
          }

          positions       = reinterpret_cast<const char*> (data) + va.offset;
          position_stride = vertex_size;
          break;
        case media::geometry::VertexAttributeSemantic_Normal:
          switch (va.type)
          {
            case media::geometry::VertexAttributeType_Float3:
            case media::geometry::VertexAttributeType_Float4:
              break;
            default:
              continue;
          }

          normals       = reinterpret_cast<const char*> (data) + va.offset;
          normal_stride = vertex_size;
          break;
        case media::geometry::VertexAttributeSemantic_Influence:
          if (va.type != media::geometry::VertexAttributeType_Influence)
            continue;
          
          vertex_influences       = reinterpret_cast<const char*> (data);
          vertex_influence_stride = vertex_size;
          break;
        default:
          break;
      }
    }
  }

  if (!vertex_influences || !positions || !src_vertex_weights) //normals are optional
    return; 

    //построение целевых вершинных весов

  tmp_skin_data.vertex_joint_weights.resize (src_vertex_weights_count);

  VertexJointWeight                    *dst_vertex_joint_weights = &tmp_skin_data.vertex_joint_weights [0],
                                       *dst_joint_weight         = dst_vertex_joint_weights;
  const media::geometry::VertexWeight* src_weight                = src_vertex_weights;
  
  for (size_t i=0; i<src_vertex_weights_count; i++, src_weight++, dst_joint_weight++)
  {
    dst_joint_weight->joint_index = src_weight->joint_index;
    dst_joint_weight->weight      = src_weight->joint_weight;
  }

    //построение influences

  tmp_skin_data.vertex_weights.resize (vertices_count);

  VertexWeight* dst_vertex_weight = &tmp_skin_data.vertex_weights [0];
  const char    *src_position_ptr = positions, *src_normal_ptr = normals, *src_infuence_ptr = vertex_influences;

  static const math::vec3f default_normal (0, 0, 1.0f);

  for (size_t i=0; i<vertices_count; i++)
  {
    const media::geometry::VertexInfluence& src_influence = *reinterpret_cast<const media::geometry::VertexInfluence*> (src_infuence_ptr);
    const math::vec3f&                      src_position  = *reinterpret_cast<const math::vec3f*> (src_position_ptr);
    const math::vec3f&                      src_normal    = src_normal_ptr ? *reinterpret_cast<const math::vec3f*> (src_normal_ptr) : default_normal;

    dst_vertex_weight->first_weight  = dst_vertex_joint_weights + src_influence.first_weight;
    dst_vertex_weight->weights_count = src_influence.weights_count;
    dst_vertex_weight->position      = src_position;
    dst_vertex_weight->normal        = src_normal;

    src_position_ptr += position_stride;
    src_infuence_ptr += vertex_influence_stride;
    src_normal_ptr    = src_normal_ptr ? src_normal_ptr + normal_stride : (const char*)0;
    dst_vertex_weight++;
  }
  
    //резервирование памяти для буфер расчета динамических вершин скин-меша

  tmp_skin_data.skin_vertices.resize (vertices_count);

    //создание контейнера данных скин меша

  skin_data.reset (new SkinData);

  skin_data->vertex_joint_weights.swap (tmp_skin_data.vertex_joint_weights);
  skin_data->vertex_weights.swap (tmp_skin_data.vertex_weights);
  skin_data->skin_vertices.swap (tmp_skin_data.skin_vertices);  
  skin_data->stream_index = -1;
}
