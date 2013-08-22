#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
    Конструкторы / деструктор
*/

VertexBuffer::VertexBuffer (const media::geometry::VertexBuffer& source, PrimitiveBuffersImpl& buffers, const DeviceManagerPtr& device_manager, MeshBufferUsage usage)
{
  try
  {
    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");

    low_level::IDevice& device = device_manager->Device ();

    InputLayoutManager& layout_manager = device_manager->InputLayoutManager ();

      //резервирование вершинных атрибутов
    
    size_t attributes_count = 0;
    
    for (size_t i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
      attributes_count += source.Stream (i).Format ().AttributesCount ();
      
    attributes.reserve (attributes_count);

    streams.reserve (source.StreamsCount ());
    vertex_formats.reserve (source.StreamsCount ());
    
      //конвертация

    attributes_hash = 0xFFFFFFFF;
    
    for (size_t i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
    {
      const media::geometry::VertexStream& vs = source.Stream (i);
      
      LowLevelBufferPtr vs_buffer = buffers.CreateVertexStream (vs, usage);

      media::geometry::VertexFormat vertex_format = Clone (layout_manager, vs.Format ());      
      size_t                        vs_hash       = vertex_format.Hash ();

      attributes_hash = common::crc32 (&vs_hash, sizeof (vs_hash), attributes_hash);

      for (size_t j=0, attr_count=vertex_format.AttributesCount (); j<attr_count; j++)
      {
        const media::geometry::VertexAttribute& src_va = vertex_format.Attribute (j);
        VertexAttribute                         dst_va;

        memset (&dst_va, 0, sizeof (dst_va));

        if (*src_va.name)
        {
          dst_va.semantic = src_va.name; //can't be changed in futured, stored in internal InputLayoutManager cache and in this object
        }
        else
        {
          switch (src_va.semantic)
          {
            case media::geometry::VertexAttributeSemantic_Position:
              dst_va.semantic = device.GetVertexAttributeSemanticName (VertexAttributeSemantic_Position);
              break;
            case media::geometry::VertexAttributeSemantic_Normal:
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
            dst_va.type   = InputDataType_Short;
            dst_va.format = InputDataFormat_Vector2;
            break;
          case media::geometry::VertexAttributeType_Short3:
            dst_va.type   = InputDataType_Short;
            dst_va.format = InputDataFormat_Vector3;
            break;
          case media::geometry::VertexAttributeType_Short4:
            dst_va.type   = InputDataType_Short;
            dst_va.format = InputDataFormat_Vector4;
            break;
          case media::geometry::VertexAttributeType_UByte4:
            dst_va.type   = InputDataType_UByte;
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

size_t VertexBuffer::StreamsCount ()
{
  return streams.size ();
}

const LowLevelBufferPtr* VertexBuffer::Streams ()
{
  if (streams.empty ())
    return 0;
    
  return &streams [0];
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

    layout_desc.vertex_attributes_count = attributes.size ();
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
