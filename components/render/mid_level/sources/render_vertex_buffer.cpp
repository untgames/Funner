#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Конструкторы / деструктор
*/

VertexBuffer::VertexBuffer (const media::geometry::VertexBuffer& source, PrimitiveBuffersImpl& buffers, MeshBufferUsage usage)
{
  try
  {
      //резервирование вершинных атрибутов
    
    size_t attributes_count = 0;
    
    for (size_t i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
      attributes_count += source.Stream (i).Format ().AttributesCount ();
      
    attributes.reserve (attributes_count);
    
      //конвертация
    
    for (size_t i=0, streams_count=source.StreamsCount (); i<streams_count; i++)
    {
      const media::geometry::VertexStream& vs = source.Stream (i);
      
      LowLevelBufferPtr vs_buffer = buffers.CreateVertexStream (vs, usage);

      const media::geometry::VertexFormat& vertex_format = vs.Format ();

      for (size_t j=0, attr_count=vertex_format.AttributesCount (); j<attr_count; j++)
      {
        const media::geometry::VertexAttribute& src_va = vertex_format.Attribute (j);
        VertexAttribute                         dst_va;

        memset (&dst_va, 0, sizeof (dst_va));

        switch (src_va.semantic)
        {
          case media::geometry::VertexAttributeSemantic_Position:
            dst_va.semantic = VertexAttributeSemantic_Position;
            break;
          case media::geometry::VertexAttributeSemantic_Normal:
            dst_va.semantic = VertexAttributeSemantic_Normal;
            break;
          case media::geometry::VertexAttributeSemantic_Color:
            dst_va.semantic = VertexAttributeSemantic_Color;
            break;
          case media::geometry::VertexAttributeSemantic_TexCoord0:
          case media::geometry::VertexAttributeSemantic_TexCoord1:
          case media::geometry::VertexAttributeSemantic_TexCoord2:
          case media::geometry::VertexAttributeSemantic_TexCoord3:
          case media::geometry::VertexAttributeSemantic_TexCoord4:
          case media::geometry::VertexAttributeSemantic_TexCoord5:
          case media::geometry::VertexAttributeSemantic_TexCoord6:
          case media::geometry::VertexAttributeSemantic_TexCoord7:          
            dst_va.semantic = (VertexAttributeSemantic)(VertexAttributeSemantic_TexCoord0 + src_va.semantic - media::geometry::VertexAttributeSemantic_TexCoord0);
            break;
//          case media::geometry::VertexAttributeSemantic_Tangent:
//            dst_va.semantic = VertexAttributeSemantic_TexCoord3;
//            break;
          default:
            continue;
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
    e.touch ("render::mid_level::VertexBuffer::VertexBuffer");
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

LowLevelInputLayoutPtr VertexBuffer::CreateInputLayout (render::low_level::IDevice& device, InputDataType type)
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

    layout = LowLevelInputLayoutPtr (device.CreateInputLayout (layout_desc), false);
      
    return layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::VertexBuffer::CreateInputLayout");
    throw;
  }
}
