#include "shared.h"

using namespace media::geometry;
using namespace common;
using namespace math;

namespace
{

const float EPS = 1e-06f;

/*
    Протоколирование
*/

class Log
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;
    
    Log (const Mesh& in_mesh, const LogHandler& in_log_handler) : mesh (in_mesh), log_handler (in_log_handler), has_errors (false) {}
    
    bool HasErrors () const { return has_errors; }
    
    void Error (const char* format_message, ...)
    {
      va_list list;

      va_start (list, format_message);

      has_errors = true;

      log_handler (format ("mesh '%s': %s", mesh.Name (), vformat (format_message, list).c_str ()).c_str ());
    }
    
  private:
    const Mesh&  mesh;
    LogHandler   log_handler;
    bool         has_errors;
};

/*
    Проверка корректности вершинного потока
*/

struct CheckContext
{
  const VertexStream&        stream;              //вершинный массив
  const VertexWeightStream&  weights;             //веса
  unsigned int               vertex_buffer_index; //номер вершинного буфера
  unsigned int               stream_index;        //номер вершинного массива
  unsigned int               weights_count;       //количество весов
  unsigned int               vertex_index;        //номер вершины
  const VertexAttribute*     attribute;
  
  CheckContext (const VertexBuffer& vb, unsigned int in_vertex_buffer_index, unsigned int in_stream_index) :
    stream (vb.Stream (in_stream_index)), weights (vb.Weights ()), vertex_buffer_index (in_vertex_buffer_index), stream_index (in_stream_index),
    weights_count (weights.Size ()), vertex_index (0), attribute (0) {}
};

inline void check_attribute (Log& log, CheckContext& context, const VertexInfluence& influence)
{
  if (!influence.weights_count)
    return;

  if (influence.first_weight >= context.weights_count)
  {
    log.Error ("vertex_buffer[%u].stream[%u].vertex[%u].first_weight=%u > weights_count=%u",
               context.vertex_buffer_index, context.stream_index, context.vertex_index, influence.first_weight, context.weights_count);
               
    return;
  }
  
  if (context.weights_count - influence.first_weight < influence.weights_count)  
  {
    log.Error ("vertex_buffer[%u].stream[%u].vertex[%u].last_weight=%u > weights_count=%u (last_weight=first_weight+weights_count)",
               context.vertex_buffer_index, context.stream_index, context.vertex_index, influence.first_weight + influence.weights_count, context.weights_count);
               
    return;    
  }
  
  const VertexWeight* weight      = context.weights.Data () + influence.first_weight;
  float               weights_sum = 0.0f;
  
  for (unsigned int i=0; i<influence.weights_count; i++, weight++)
    weights_sum += weight->joint_weight;
    
  if (fabs (weights_sum - 1.0f) > EPS)
    log.Error ("vertex_buffer[%u].stream[%u].vertex[%u] not normalized (weights_sum=%.3f)",
               context.vertex_buffer_index, context.stream_index, context.vertex_index, weights_sum);
}

template <unsigned int Size>
inline void check_attribute (Log& log, CheckContext& context, const math::vector<float, Size>& attribute)
{
  static const char* component_name [] = {"x", "y", "z", "w"};

  for (unsigned int i=0; i<Size; i++)
  {
    #ifdef _MSC_VER
      #define isfinite _finite
    #else
      using namespace std;
    #endif
    
    if (!isfinite (attribute [i]))
      log.Error ("infinite value vertex_buffer[%u].stream[%u].vertex[%u].%s.%s (attribute_type='%s')",
                 context.vertex_buffer_index, context.stream_index, context.vertex_index, get_semantic_name (context.attribute->semantic), component_name [i],
                 get_type_name (context.attribute->type));
  }
}

template <class T>
void check_stream (Log& log, CheckContext& context, const char* iter)
{
  unsigned int verts_count = context.stream.Size (),
               stride      = context.stream.VertexSize ();

  for (unsigned int i=0; i<verts_count; i++, iter += stride)
  {
    context.vertex_index = i;
    check_attribute (log, context, *reinterpret_cast<const T*> (iter));
  }
}

void check_stream (Log& log, CheckContext& context)
{  
  const VertexFormat& format = context.stream.Format ();
    
  for (unsigned int i=0, count=format.AttributesCount (); i<count; i++)
  {
    const VertexAttribute& attribute = format.Attribute (i);

    switch (attribute.semantic)
    {
      case VertexAttributeSemantic_Position:
      case VertexAttributeSemantic_Normal:
      case VertexAttributeSemantic_Color:
      case VertexAttributeSemantic_TexCoord0:
      case VertexAttributeSemantic_TexCoord1:
      case VertexAttributeSemantic_TexCoord2:
      case VertexAttributeSemantic_TexCoord3:
      case VertexAttributeSemantic_TexCoord4:
      case VertexAttributeSemantic_TexCoord5:
      case VertexAttributeSemantic_TexCoord6:
      case VertexAttributeSemantic_TexCoord7:
      case VertexAttributeSemantic_Influence:
      case VertexAttributeSemantic_Custom:
        break;
      default:
        log.Error ("vertex_buffer[%u].stream[%u].format.attribute[%u] has unknown semantic=%d",
                   context.vertex_buffer_index, context.stream_index, i, attribute.semantic);
        continue;
    }
    
    switch (attribute.type)
    {
      case VertexAttributeType_Float2:
      case VertexAttributeType_Float3:
      case VertexAttributeType_Float4:
      case VertexAttributeType_Influence:
      case VertexAttributeType_Short2:
      case VertexAttributeType_Short3:
      case VertexAttributeType_Short4:
      case VertexAttributeType_UByte4:
        break;
      default:
        log.Error ("vertex_buffer[%u].stream[%u].format.attribute[%u] has unknown type=%d",
                   context.vertex_buffer_index, context.stream_index, i, attribute.type);
        continue;
    }
    
    if (!is_compatible (attribute.semantic, attribute.type))
    {
      log.Error ("vertex_buffer[%u].stream[%u].format.attribute[%u] has semantic '%s' incompatible with type '%s'",
                 context.vertex_buffer_index, context.stream_index, i, get_semantic_name (attribute.semantic), get_type_name (attribute.type));

      continue;
    }

    unsigned int vertex_size = context.stream.VertexSize (),
                 type_size   = get_type_size (attribute.type);
    
    if (attribute.offset > vertex_size || vertex_size - attribute.offset < type_size)
    {
       log.Error ("bad offset vertex_buffer[%u].stream[%u].format.attribute[%u].offset=%u (type_size=%u, vertex_size=%u)",
                  context.vertex_buffer_index, context.stream_index, i, attribute.offset, type_size, vertex_size);
                  
       continue;
    }

    const char* data = static_cast<const char*> (context.stream.Data ()) + attribute.offset;
    
    context.attribute = &attribute;

    switch (attribute.type)
    {
      case VertexAttributeType_Float2:
        check_stream<vec2f> (log, context, data);
        break;
      case VertexAttributeType_Float3:
        check_stream<vec3f> (log, context, data);
        break;
      case VertexAttributeType_Float4:
        check_stream<vec4f> (log, context, data);
        break;
      case VertexAttributeType_Influence:
        check_stream<VertexInfluence> (log, context, data);
        break;
      default:
        break;
    }
  }
}

template <class T>
void check_indices (unsigned int count, const T* index, unsigned int verts_count, unsigned int base_vertex, unsigned int primitive_index, Log& log)
{
  for (unsigned int j=0; j<count; j++, index++)
    if (*index + base_vertex >= verts_count)
    {
      if (base_vertex)
      {
        log.Error ("index[%u]=%u+%u >= vertices_count=%u (at primitive #%u)", j, *index, base_vertex, verts_count, primitive_index);
      }
      else
      {
        log.Error ("index[%u]=%u >= vertices_count=%u (at primitive #%u)", j, *index, verts_count, primitive_index);
      }
    }
  
}

}

/*
    Проверка корректности меша
*/

namespace media
{

namespace geometry
{

bool check (const Mesh& mesh, uint32_t joints_count, const xtl::function<void (const char*)>& log_handler)
{
  Log log (mesh, log_handler);
  
  try
  {
    const IndexBuffer& index_buffer = mesh.IndexBuffer ();
    const MaterialMap& material_map = mesh.MaterialMap ();
    
      //проверка примитивов
    
    bool has_indices = mesh.IndexBuffer ().Size () != 0;

    for (unsigned int i=0, count=mesh.PrimitivesCount (); i<count; i++)
    {                  
      const Primitive& primitive = mesh.Primitive (i);
      
      if (primitive.vertex_buffer >= mesh.VertexBuffersCount ())
      {
        log.Error ("primitive[%u].vertex_buffer=%u >= vertex_buffers_count=%u", i, primitive.vertex_buffer, mesh.VertexBuffersCount ());
        continue;
      }

      const VertexBuffer& vertex_buffer = mesh.VertexBuffer (primitive.vertex_buffer);

      unsigned int max_index      = has_indices ? index_buffer.Size () : vertex_buffer.VerticesCount ();
      const char*  max_index_name = has_indices ? "indices_count" : "vertices_count";
      
      switch (primitive.type)
      {
        case PrimitiveType_LineList:
        case PrimitiveType_LineStrip:
        case PrimitiveType_TriangleList:
        case PrimitiveType_TriangleStrip:
        case PrimitiveType_TriangleFan:
          break;
        default:
          log.Error ("primitive[%u] has unknown type=%d", i, primitive.type);
          break;
      }

      if (!material_map.MaterialName (primitive.material_id))
        log.Error ("null primitive[%u].material", i);

      if (primitive.first >= max_index)
      {
        log.Error ("primitive[%u].first=%u > %s=%u", i, primitive.first, max_index_name, max_index);
                   
        continue;
      }
      
      if (max_index - primitive.first < get_points_count (primitive.type, primitive.count))
      {
        log.Error ("primitive[%u] is out of range (first=%u, points_count=%u, %s=%u)", i, primitive.first,
                   get_points_count (primitive.type, primitive.count), max_index_name, max_index);
                   
        continue;
      }
      
        //проверка корректности индексов
        
      unsigned int verts_count = vertex_buffer.VerticesCount ();
      
      switch (index_buffer.DataType ())
      {
        case IndexType_UInt32:
          check_indices (index_buffer.Size (), index_buffer.Data<const unsigned int> (), verts_count, primitive.base_vertex, i, log);
          break;
        case IndexType_UInt16:
          check_indices (index_buffer.Size (), index_buffer.Data<const unsigned short> (), verts_count, primitive.base_vertex, i, log);
          break;
        case IndexType_UInt8:
          check_indices (index_buffer.Size (), index_buffer.Data<const unsigned char> (), verts_count, primitive.base_vertex, i, log);
          break;
        default:
          log.Error ("Bad index type %s", get_index_type_name (index_buffer.DataType ()));
          break;
      }
    }
        
      //проверка вершинных буферов
      
    for (unsigned int i=0, count=mesh.VertexBuffersCount (); i<count; i++)
    {
        //проверка вершинных массивов

      const VertexBuffer& vertex_buffer = mesh.VertexBuffer (i);

      for (unsigned int j=0, count=vertex_buffer.StreamsCount (); j<count; j++)
      {
        CheckContext check_context (vertex_buffer, i, j);
        
        check_stream (log, check_context);
      }

        //проверка массива весов
        
      const VertexWeight* weight = vertex_buffer.Weights ().Data ();
        
      for (unsigned int j=0, count=vertex_buffer.Weights ().Size (); j<count; j++, weight++)
      {
        if (weight->joint_index >= joints_count)
          log.Error ("vertex_buffer[%u].weight[%u].joint_index=%u >= joints_count=%u", i, j, weight->joint_index, joints_count);
      }        
    }
  }
  catch (std::exception& exception)
  {
    log.Error ("exception: %s", exception.what ());
    return false;    
  }
  catch (...)
  {
    log.Error ("unknown exception");
    return false;
  }
  
  return !log.HasErrors ();
}

bool check (const Mesh& mesh, const xtl::function<void (const char*)>& log_handler)
{
  return check (mesh, uint32_t (-1), log_handler);
}

}

}
