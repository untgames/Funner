#include "shared.h"

using namespace media;
using namespace media::geometry;
using namespace common;

/*
    Разбор вершинных данных
*/

namespace media
{

namespace geometry
{

template <class Token, class BaseIter>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, VertexInfluence& influence)
{
  if (iter.available () < 2)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;
  
  if (!read (iter, influence.first_weight) || !read (iter, influence.weights_count))
  {
    iter = save;
    return false;
  }

  return true;
}

template <class Token, class BaseIter>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, VertexWeight& weight)
{
  if (iter.available () < 2)
    return false;

  xtl::io::token_iterator<Token, BaseIter> save = iter;
  
  if (!read (iter, weight.joint_index) || !read (iter, weight.joint_weight))
  {
    iter = save;
    return false;
  }

  return true;
}

}

}

namespace
{

/*
    Загрузчик меш-моделей в формате Xml
*/

class XmlMeshModelLoader
{
  private:
      //разбор вершинного формата
    void ParseVertexFormatChannel (Parser::Iterator channel_iter, VertexFormat& vertex_format)
    {
      VertexAttributeSemantic semantic = get_vertex_attribute_semantic (get<const char*> (channel_iter, "semantic", ""));
      VertexAttributeType     type     = get_vertex_attribute_type (get<const char*> (channel_iter, "type", ""));
      size_t                  offset   = get<size_t> (channel_iter, "offset", (size_t)-1);

      if (semantic == VertexAttributeSemantic_Num || type == VertexAttributeType_Num || offset == (size_t)-1)
        return;

      vertex_format.AddAttribute (semantic, type, offset);
    }
    
      //получение итератора на данные вершинного потока
    template <class T> static xtl::stride_ptr<T> MakeVertexAttributeIterator (VertexStream& vs, size_t offset)
    {
      return xtl::stride_ptr<T> (reinterpret_cast<T*> (static_cast<char*> (vs.Data ()) + offset), vs.VertexSize ());
    }
    
      //разбор вершинного потока
    void ParseVertexStream (Parser::Iterator vs_iter)
    {     
        //чтение идентификатора потока
        
      const char* id = get<const char*> (vs_iter, "id");
      
      if (!id)
        return;
      
        //определение числа вершин и размера вершины
      
      size_t vertices_count = get<size_t> (vs_iter, "vertices_count"),
             vertex_size    = get<size_t> (vs_iter, "vertex_size");

      if (!vertices_count || !vertex_size)
        return;

      VertexFormat vertex_format;

        //разбор вершинного формата
      
      for_each_child (vs_iter, "channel", xtl::bind (&XmlMeshModelLoader::ParseVertexFormatChannel, this, _1, xtl::ref (vertex_format)));
      
        //создание вершинного потока        
      
      VertexStream vs (vertices_count, VertexDeclaration (vertex_format, vertex_size));
      
        //чтение данных
        
      Parser::NamesakeIterator channel_iter = vs_iter->First ("channel");
        
      for (size_t i=0, channel_count=vertex_format.AttributesCount (); i<channel_count; i++, ++channel_iter)
      {
        const VertexAttribute& attribute = vertex_format.Attribute (i);

        for (;channel_iter && get<size_t> (channel_iter, "offset", (size_t)-1) != attribute.offset; ++channel_iter);

        if (!channel_iter)
          return;

        switch (attribute.type)
        {
          case VertexAttributeType_Float2:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec2f> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;
          case VertexAttributeType_Float3:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec3f> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;                      
          case VertexAttributeType_Float4:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec4f> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;          
          case VertexAttributeType_Short2:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec2s> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;          
          case VertexAttributeType_Short3:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec3s> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;          
          case VertexAttributeType_Short4:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec4s> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;          
          case VertexAttributeType_UByte4:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<math::vec4ub> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;          
          case VertexAttributeType_Influence:
            if (read_range (channel_iter, "#text", MakeVertexAttributeIterator<VertexInfluence> (vs, attribute.offset), vertices_count) != vertices_count)
              return;

            break;          
          default:
            return;
        }
      }

        //регистрация потока

      vertex_streams.insert_pair (id, vs);
    }
    
      //разбор потока вершинных весов
    void ParseVertexWeightStream (Parser::Iterator vws_iter)
    {
        //чтение идентификатора потока
        
      const char* id = get<const char*> (vws_iter, "id");
      
      if (!id)
        return;
        
      size_t weights_count = get<size_t> (vws_iter, "weights_count");
      
      if (!weights_count)
        return;
        
        //создание потока
      
      VertexWeightStream vws (weights_count);

        //чтение весов
        
      if (read_range (vws_iter, "#text", vws.Data (), weights_count) != weights_count)
        return;
        
        //регистрация потока
        
      vertex_weights.insert_pair (id, vws);
    }

      //разбор вершинного буфера
    void ParseVertexBuffer (Parser::Iterator vb_iter)
    {
        //чтение идентификатора буфера
        
      const char* id = get<const char*> (vb_iter, "id");
      
      if (!id)
        return;
        
         //создание вершинного буфера

      VertexBuffer vb;
      
         //разбор потока вершинных весов
         
      const char* weights_id = get<const char*> (vb_iter, "weights");
       
      if (weights_id)
      {
        VertexWeightStreamMap::iterator weights_iter = vertex_weights.find (weights_id);
        
        if (weights_iter == vertex_weights.end ())
          return;
          
        vb.AttachWeights (weights_iter->second);
      }

         //разбор вершинных потоков
         
      for (Parser::AttributeIterator i=make_attribute_iterator (vb_iter, "streams.#text"); i; ++i)
      {
        const char* id = *i;
        
        if (!id)
          continue;         

        VertexStreamMap::iterator stream_iter = vertex_streams.find (id);

        if (stream_iter == vertex_streams.end ())
          continue;

        vb.Attach (stream_iter->second);
      }

         //регистрация вершинного буфера

      vertex_buffers.insert_pair (id, vb);
    }
    
      //разбор индексного буфера
    void ParseIndexBuffer (Parser::Iterator ib_iter)
    {
        //чтение идентификатора буфера
        
      const char* id = get<const char*> (ib_iter, "id");
      
      if (!id)
        return;
        
      size_t indices_count = get<size_t> (ib_iter, "indices_count");
      
      if (!indices_count)
        return;
        
        //создание буфера
      
      IndexBuffer ib (indices_count);

        //чтение индексов
        
      if (read_range (ib_iter, "#text", ib.Data (), indices_count) != indices_count)
        return;
        
        //регистрация потока
        
      index_buffers.insert_pair (id, ib);
    }
    
      //разбор примитива
    void ParsePrimitive (Parser::Iterator primitive_iter, Mesh& mesh)
    {
      PrimitiveType type     = get_primitive_type (get<const char*> (primitive_iter, "type", ""));
      const char*   material = get<const char*> (primitive_iter, "material", "");
      size_t        first    = get<size_t> (primitive_iter, "first", (size_t)-1),
                    count    = get<size_t> (primitive_iter, "count", (size_t)-1);
                    
      if (type == PrimitiveType_Num || first == (size_t)-1 || count == (size_t)-1)
        return;

      mesh.AddPrimitive (type, first, count, material);
    }
    
      //разбор мешей
    void ParseMesh (Parser::Iterator mesh_iter)
    {
        //чтение имён: меша, вершинного буфера, индексного буфера (если есть)
      
      const char *name  = get<const char*> (mesh_iter, "name", ""),
                 *vb_id = get<const char*> (mesh_iter, "vertex_buffer"),
                 *ib_id = get<const char*> (mesh_iter, "index_buffer");
                 
        //поиск вершинного и индексного буфера
        
      VertexBuffer* vb = 0;
      IndexBuffer*  ib = 0;

      if (!vb_id)
        return;
        
      VertexBufferMap::iterator vb_iter = vertex_buffers.find (vb_id);
      
      if (vb_iter == vertex_buffers.end ())
        return;        
        
      vb = &vb_iter->second;
      
      if (ib_id)
      {
        IndexBufferMap::iterator ib_iter = index_buffers.find (ib_id);
        
        if (ib_iter == index_buffers.end ())
          return;
          
        ib = &ib_iter->second;
      }
      
        //создание меша
        
      Mesh mesh;
      
      mesh.Rename (name);

      mesh.Attach (*vb);

      if (ib)
        mesh.Attach (*ib);              
        
        //чтение примитивов

      for_each_child (mesh_iter, "primitive", xtl::bind (&XmlMeshModelLoader::ParsePrimitive, this, _1, xtl::ref (mesh)));      

        //присоединение меша к модели

      model.Attach (mesh);            
    }
  
      //разбор модели
    void ParseModel (Parser::Iterator model_iter)
    {     
      for_each_child (model_iter, "vertex_streams.vertex_stream", xtl::bind (&XmlMeshModelLoader::ParseVertexStream, this, _1));
      for_each_child (model_iter, "vertex_streams.vertex_weight_stream", xtl::bind (&XmlMeshModelLoader::ParseVertexWeightStream, this, _1));
      for_each_child (model_iter, "vertex_buffers.vertex_buffer", xtl::bind (&XmlMeshModelLoader::ParseVertexBuffer, this, _1));
      for_each_child (model_iter, "index_buffers.index_buffer", xtl::bind (&XmlMeshModelLoader::ParseIndexBuffer, this, _1));      
      for_each_child (model_iter, "meshes.mesh", xtl::bind (&XmlMeshModelLoader::ParseMesh, this, _1));
    }

  public:
    XmlMeshModelLoader (const char* file_name, MeshModel& in_model) : parser (file_name, "xml"), model (in_model)
    {
      ParseModel (parser.Root ()->First ("model"));
    }
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, VertexStream>       VertexStreamMap;
    typedef stl::hash_map<stl::hash_key<const char*>, VertexWeightStream> VertexWeightStreamMap;
    typedef stl::hash_map<stl::hash_key<const char*>, VertexBuffer>       VertexBufferMap;
    typedef stl::hash_map<stl::hash_key<const char*>, IndexBuffer>        IndexBufferMap;

  private:
    Parser                parser;         //парсер
    MeshModel&            model;          //модель
    VertexStreamMap       vertex_streams; //загруженные вершинные потоки
    VertexWeightStreamMap vertex_weights; //загруженные потоки вершинных весов
    VertexBufferMap       vertex_buffers; //загруженные вершинные буферы
    IndexBufferMap        index_buffers;  //загруженные индексные буферы
};

}

namespace media
{

namespace geometry
{

/*
    Загрузка меш-модели
*/

void xmesh_load_model (const char* file_name, MeshModel& model)
{
  XmlMeshModelLoader (file_name, model);
}

}

}
