#include <stl/vector>
#include <stl/hash_map>
#include <stl/string>
#include <common/hash.h>

#include "shared.h"

using namespace medialib::collada;

/*
    Вспомогательные структуры данных
*/

namespace medialib
{

namespace collada
{

typedef stl::vector<float> FloatBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Источник данных меша
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MeshSource
{
  FloatBuffer data;   //данные
  stl::string params; //параметры
  size_t      count;  //количество элементов
  size_t      stride; //шаг
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Карта отображения имени источника в указатель на него
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshSourceMap
{
  public:
      //создание источника данных меша
    MeshSource* Create (const char* name)
    {
      return &sources [name];
    }
    
      //удаление источника данных меша
    void Remove (const char* name)
    {
      sources.erase (name);
    }
    
      //поиск источника данных меша
    MeshSource* Find (const char* name)
    {
      Sources::iterator iter = sources.find (name);
      
      if (iter == sources.end ())
        return 0;
 
      return &iter->second;
    }
  
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, MeshSource> Sources;
    
    Sources sources;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Канал данных меша
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MeshInput
{
  Parser::Node* node;   //узел дерева разбора
  MeshSource*   source; //источник данных
  size_t        offset; //смещение в массиве индексов
  
  MeshInput () : node (0), source (0), offset (0) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс, создающий каналы данных меша
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshInputBuilder
{
  enum
  {
    DEFAULT_INPUTS_RESERVE = 32,
    DEFAULT_SETS_RESERVE   = 8
  };
  
  public:   
    MeshInputBuilder ()
    {       
      inputs.reserve (DEFAULT_INPUTS_RESERVE);
      sets.reserve (DEFAULT_SETS_RESERVE);
    }
    
      //добавление канала данных меша
    bool AddChannel (const char* semantic, const char* set, Parser::Node* node, MeshSource* source, size_t offset)
    {
      if (!set)
        set = "";

      size_t                 hash = strhash (semantic, common::strhash (set));
      MeshInputMap::iterator iter = input_map.find (hash);
      
      if (iter != input_map.end ())
        return false; //этот канал уже был зарегистрирован

      inputs.push_back ();

      MeshInput& input = inputs.back ();

      input.node   = node;
      input.source = source;
      input.offset = offset;
      
      try
      {       
        input_map [hash] = inputs.size () - 1;
        
        try
        {
          RegisterSet (set);          
          
          return true;          
        }
        catch (...)
        {
          throw;
        }
      }
      catch (...)
      {
        inputs.pop_back ();
        throw;
      }      
    }
    
      //поиск канала
    const MeshInput* FindChannel (const char* semantic, const char* set=0)
    {
      if (!set)
        set = "";

      size_t                 hash = strhash (semantic, common::strhash (set));
      MeshInputMap::iterator iter = input_map.find (hash);

      return iter != input_map.end () ? &inputs [iter->second] : 0;
    }
    
      //получение количества каналов данных
    size_t GetChannelsCount  () const { return inputs.size (); }
    
      //количество подмножеств
    size_t GetSetsCount () const { return sets.size (); }
    
      //поулчение имени подмножества
    const char* GetSetName (size_t index) const { return sets [index]; }

  private:
    void RegisterSet (const char* set)
    {
      if (!set || !*set)
        return;
        
      for (SetArray::iterator i=sets.begin (), end=sets.end (); i!=end; ++i)
        if (!::strcmp (*i, set))
          return;
          
      sets.push_back (set);
    }

  private:
    typedef stl::hash_map<size_t, size_t> MeshInputMap;
    typedef stl::vector<MeshInput>        MeshInputArray;
    typedef stl::vector<const char*>      SetArray;
    
  private:
    MeshInputArray inputs;
    MeshInputMap   input_map;
    SetArray       sets;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершинный буфер меша
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshVertexBuffer
{
  public:
    MeshVertexBuffer (size_t in_inputs_count, size_t reserve_vertices_count)
    {
      inputs_count = in_inputs_count;
      
      vertices.reserve (reserve_vertices_count);
    }
    
      //добавление вершины
    size_t AddVertex (size_t* inputs)
    {
        //определение хэша вершины по набору индексов входных каналов данных (номер в буфере позиций, номер в буфере нормалей, ...)

      size_t vertex_hash = GetVertexHash (inputs);

        //проверка: существует ли такая вершина в буфере вершин

      VertexBufferMap::iterator search_iter = vertices_map.find (vertex_hash);

      if (search_iter != vertices_map.end ())
        return search_iter->second;

        //если вершины нет в буфере - добавление её в буфер вершин и карту вершин

      size_t vertex_index = vertices.size ();

      vertices.push_back (inputs);

      try
      {
        vertices_map [vertex_hash] = vertex_index;

        return vertex_index;
      }
      catch (...)
      {
        vertices.pop_back ();
        throw;
      }      
    }
    
      //количество вершин в буфере
    size_t GetVerticesCount () const { return vertices.size (); }
    
      //получение массива вершин
    size_t** GetVertices () { return &vertices [0]; }

  private:
    size_t GetVertexHash (const size_t* inputs)
    {
      return common::crc32 (inputs, sizeof (size_t) * inputs_count);
    }
  
  private:
    typedef stl::hash_map<size_t, size_t> VertexBufferMap;
    typedef stl::vector<size_t*>          VertexBuffer;  
    
    VertexBuffer    vertices;
    VertexBufferMap vertices_map;
    size_t          inputs_count;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация для создания поверхности
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SurfaceInfo
{
  Mesh*             mesh;
  Material*         material;
  PrimitiveType     primitive_type;
  MeshInputBuilder  inputs;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс упрощающий чтение каналов вершинных данных
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexStreamReader
{
  public:
    VertexStreamReader (DaeParser&        in_parser,
                        Parser::Node*     in_surface_node,
                        MeshInputBuilder& in_inputs,
                        MeshVertexBuffer& in_vertex_buffer)
       : parser (in_parser), surface_node (in_surface_node), inputs (in_inputs), vertex_buffer (in_vertex_buffer) {}

      //чтение канала данных в поле, определеяемое указателем на член-класса
    template <class T, class Field>
    bool Read (const char* semantic, const char* set, const char* params, T* buffer, Field T::* field)
    {
      return ReadCore (semantic, set, params, buffer, field_selector<T, Field> (field));
    }

      //непосредственное чтение канала данных    
    template <class T>
    bool Read (const char* semantic, const char* set, const char* params, T* buffer)
    {
      return ReadCore (semantic, set, params, buffer, identity_selector ());
    }    
    
  private:
      //функтор возвращающий член класса
    template <class T, class Field> struct field_selector
    {
      field_selector (Field T::* in_field) : field (in_field) {}
      
      Field& operator () (T& object) const { return object.*field; }
      
      Field T::* field;
    };
    
      //функтор возвращающий сам элемент
    struct identity_selector
    {
      template <class T> T& operator () (T& object) const { return object; }
    };
    
  
      //чтение канала данных
    template <class T, class Fn>  
    bool ReadCore (const char* semantic, const char* set, const char* params, T* buffer, Fn fn)
    {
      const MeshInput* input = inputs.FindChannel (semantic, set);
      
      if (!input)
      {
        if (set && *set) parser.LogError (surface_node, "No input channel with semantic='%s' and set='%s'", semantic, set);
        else             parser.LogError (surface_node, "No input channel with semantic='%s'", semantic);

        return false;
      }
      
      LogScope scope (input->node, parser, semantic);

      if (input->source->params != params)
      {
        parser.LogError (input->node, "Wrong params '%s'. Must be '%s'", input->source->params.c_str (), params);
        return false;
      }

      const float* source         = &input->source->data [0];
      size_t       max_count      = input->source->count,
                   offset         = input->offset,
                   stride         = input->source->stride,
                   vertices_count = vertex_buffer.GetVerticesCount ();
                   
      if (offset >= inputs.GetChannelsCount ())
      {
        parser.LogError (input->node, "Offset %u is greater of inputs count %u", offset, inputs.GetChannelsCount ());
        return false;
      }
      
      size_t** input_vertex  = vertex_buffer.GetVertices ();
      T*       output_vertex = buffer;  

      for (size_t i=0; i<vertices_count; i++, input_vertex++, output_vertex++)
      {
        size_t index = (*input_vertex) [offset];
        
        if (index >= max_count)
        {
          parser.LogError (surface_node->First ("p"), "Wrong index %u (max_count=%u)", index, max_count);
          return false;
        }
           
        SetField (source + index * stride, fn (*output_vertex));
      }
      
      return true;      
    }  
    
  private:
      //чтение вектора
    template <size_t N>
    void SetField (const float* src, math::vec<float, N>& res)
    {
      for (size_t i=0; i<N; i++)
        res [i] = src [i];
    }  
  
  private:
    DaeParser&        parser;
    Parser::Node*     surface_node;
    MeshInputBuilder& inputs;
    MeshVertexBuffer& vertex_buffer;
};

}

}

/*
    Разбор меша
*/

void DaeParser::ParseMesh (Parser::Iterator iter, Mesh& mesh)
{
  if (!test (iter, "vertices"))
  {
    LogError (iter, "No vertices information");
    return;
  }
  
  if (!test (iter, "vertices.input"))
  {
    LogError (iter, "Incorrect vertices information, 'input' URL not detected");
    return;
  }
  
    //чтение источников данных
    
  MeshSourceMap sources;  
    
  for_each_child (iter, "source", bind (&DaeParser::ParseMeshSource, this, _1, ref (sources)));

    //чтение поверхностей

  for_each_child (iter, "polygons",  bind (&DaeParser::LogError, this, _1, "Polygons are not supported"));
  for_each_child (iter, "polylist",  bind (&DaeParser::LogError, this, _1, "Polygons are not supported"));
  for_each_child (iter, "triangles", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleList, ref (sources)));
  for_each_child (iter, "tristrips", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleStrip, ref (sources)));
  for_each_child (iter, "trifans",   bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleFan, ref (sources)));
  for_each_child (iter, "lines",     bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_LineList, ref (sources)));
  for_each_child (iter, "linestrip", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_LineStrip, ref (sources)));
}

/*
    Разбор источника
*/

void DaeParser::ParseMeshSource (Parser::Iterator iter, MeshSourceMap& sources)
{
  const char* id = get<const char*> (iter, "id");
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }  
  
  LogScope scope (iter, *this);
  
  size_t data_count = 0, count = 0, stride = 0;
  
  if (!CheckedRead (iter, "float_array.count", data_count))
  {
    LogError (iter, "Error at read 'float_array.count'");
    return;
  }
  
  Parser::Iterator accessor_iter = iter->First ("technique_common.accessor");
  
  if (!accessor_iter || !CheckedRead (accessor_iter, "count", count) || !CheckedRead (accessor_iter, "stride", stride))
  {
    LogError (iter, "Error at read 'technique_common.accessor'");
    return;    
  }
  
  if (data_count < count * stride)
  {
    LogError (accessor_iter, "Wrong count/stride attribute. count * stride > float_array.count. count=%u, stride=%u, "
              "float_array.count=%u", count, stride, data_count);
    return;
  }

  MeshSource* source = sources.Create (id);  

  source->count  = count;
  source->stride = stride;

  source->data.resize (data_count);

  if (read_range (iter, "float_array.#text", source->data.begin (), data_count) != data_count)
  {
    LogError (iter, "Error at read 'float_array' items");

    sources.Remove (id);

    return;
  }
  
  for (Parser::NamesakeIterator param_iter=accessor_iter->First ("param"); param_iter; ++param_iter)
    source->params += get<const char*> (param_iter, "name");
}

/*
    Разбор поверхностей
*/

void DaeParser::ParseSurface
 (Parser::Iterator iter,
  Parser::Iterator mesh_iter,
  Mesh&            mesh,
  PrimitiveType    primitive_type,
  MeshSourceMap&   sources)
{
  LogScope scope (iter, *this);
  
  SurfaceInfo surface_info;
  
  surface_info.mesh           = &mesh;
  surface_info.primitive_type = primitive_type;
  
    //получение имени материала
    
  const char* material_name = get<const char*> (iter, "material");
  
  if (!material_name)
  {
    LogError (iter, "No 'material' attribute detected");
    return;
  }
  
  surface_info.material = model.Materials ().Find (material_name);
  
  if (!surface_info.material)
  {
    LogError (iter, "Material '%s' not found", material_name);
    return;
  }  

    //загрузка каналов данных поверхности

  for_each_child (iter, "input", bind (&DaeParser::ParseSurfaceInput, this, _1, mesh_iter, ref (sources), ref (surface_info.inputs)));
  
    //загрузка буферов поверхности

  for_each_child (iter, "p", bind (&DaeParser::ParseSurfaceBuffers, this, _1, iter, ref (surface_info)));
}

/*
    Разбор входных каналов данных поверхности
*/

void DaeParser::ParseSurfaceInput
 (Parser::Iterator  iter,
  Parser::Iterator  mesh_iter,
  MeshSourceMap&    sources,
  MeshInputBuilder& inputs)
{
  const char *semantic    = get<const char*> (iter, "semantic"),
             *source_name = get<const char*> (iter, "source");

  if (!semantic)
  {
    LogError (iter, "No 'semantic' in surface input node");
    return;
  }
           
  if (!source_name)
  {
    LogError (iter, "No 'source' in surface input node");
    return;
  }
  
  size_t offset = 0;
  
  if (!CheckedRead (iter, "offset", offset))
  {
    LogError (iter, "Error at read 'offset' in surface input node");
    return;
  }
 
  source_name++; //избавляемся от префиксного '#'
  
  MeshSource* source = 0;
  
  if (::strcmp (semantic, "VERTEX"))
  {
    source = sources.Find (source_name);

    if (!source)
    {
      LogError (iter, "No 'source' tag with id='%s' referenced in surface input node", source_name);
      return;
    }
  }
  else //добавление канала вершин
  {
    for (Parser::NamesakeIterator verts_iter=mesh_iter->First ("vertices"); verts_iter; ++verts_iter)
    {
      const char* id = get<const char*> (verts_iter, "id");
      
      if (id && !::strcmp (id, source_name))
      {
        const char *semantic    = get<const char*> (verts_iter, "input.semantic"),
                   *source_name = get<const char*> (verts_iter, "input.source");
                   
        if (!semantic)
        {
          LogError (verts_iter, "No 'input.semantic' in vertices input node");
          return;
        }

        if (!source_name)
        {
          LogError (verts_iter, "No 'input.source' in vertices input node");
          return;
        }
        
        if (::strcmp (semantic, "POSITION"))
        {
          LogError (verts_iter, "Wrong semantic '%s' in vertices input node. Must be 'POSITION'", semantic);
          return;
        }
        
        source_name++; //избавляемся от префиксного '#'
        
        source = sources.Find (source_name);

        if (!source)
        {
          LogError (verts_iter, "No 'input.source' tag with id='%s' referenced in vertices input node", source_name);
          return;
        }
      }
    }

    if (!source)
    {
      LogError (iter, "No 'vertices' tag with id='%s' referenced in input surface node", source_name);
      return;
    }
  }
  
    //регистрация канала данных
    
  const char* set = get<const char*> (iter, "set");
    
  if (!inputs.AddChannel (semantic, set, iter, source, offset))
  {
    if (set) LogError (iter, "Input channel '%s' from set '%s' already registered. Will be ignored", semantic, set);
    else     LogError (iter, "Input channel '%s' already registered. Will be ignored", semantic);

    return;    
  }
}

/*
    Построение буферов поверхности
      Данная функция строит вершинный и индексный буферы поверхности на основе входных каналов данных
*/

void DaeParser::ParseSurfaceBuffers(Parser::Iterator p_iter, Parser::Iterator surface_iter, SurfaceInfo& surface_info)
{  
    //получение количества индексов
  
  size_t indices_count = 0;
  
  if (!CheckedRead (surface_iter, "count", indices_count))
  {
    LogError (surface_iter, "Error at read 'count' attribute");
    return;
  }  

    //чтение исходного буфера индексов

  typedef stl::vector<size_t> IndexBuffer;

  IndexBuffer input_indices;

  size_t inputs_count        = surface_info.inputs.GetChannelsCount ();
  size_t input_indices_count = indices_count * inputs_count;

  input_indices.resize (input_indices_count);  

  if (read_range (p_iter, "#text", &input_indices [0], input_indices_count) != input_indices_count)
  {
    LogError (p_iter, "Wrong index buffer");
    return;
  }
  
    //построение исходного вершинного буфера (каждый элемент - указатель на массив индексов во входных каналах данных)
    //резервирование максимально возможного размера буфера вершин и буфера индексов    
  
  MeshVertexBuffer vertex_buffer (inputs_count, input_indices_count);

    //построение результирующего буфера индексов и исходный буфер вершин
    
  IndexBuffer output_indices;
  
  output_indices.resize (indices_count);
    
  size_t* index = &output_indices [0];  

  for (IndexBuffer::iterator index_iter=input_indices.begin (), index_end=input_indices.end (); index_iter != index_end;
       index_iter += inputs_count, index++)
  {    
    *index = vertex_buffer.AddVertex (&*index_iter);
  }  
  
    //создание поверхности
    
  Mesh::SurfaceList& surfaces = surface_info.mesh->Surfaces ();
    
  Surface& surface = surfaces.Create (*surface_info.material, surface_info.primitive_type,
                                      vertex_buffer.GetVerticesCount (), indices_count);
  
    //копирование буфера индексов

  stl::copy (output_indices.begin (), output_indices.end (), surface.Indices ());

    //создание объекта, читающего каналы вершинных данных  

  VertexStreamReader stream_reader (*this, surface_iter, surface_info.inputs, vertex_buffer);
 
    //построение результирующего буфера вершин
    
  Vertex* vertices = surface.Vertices ();

  if (!stream_reader.Read ("VERTEX", "", "XYZ", vertices, &Vertex::coord))
  {
    LogError (surface_iter, "Error at read VERTEX stream");
    
    surfaces.Remove (surface);
    
    return;
  }
  
  if (!stream_reader.Read ("NORMAL", "", "XYZ", vertices, &Vertex::normal))
  {
    LogError (surface_iter, "Error at read NORMAL stream");

    surfaces.Remove (surface);

    return;
  }
  
    //построение каналов текстурных координат
    
  for (size_t i=0; i<surface_info.inputs.GetSetsCount (); i++)
  {
    const char* set     = surface_info.inputs.GetSetName (i);
    
    if (surface_info.inputs.FindChannel ("TEXCOORD", set))
    {    
      size_t channel = surface.TexVertexChannels ().Create (set);
    
      TexVertex* tex_vertices = surface.TexVertexChannels ().Data (channel);      
      
      if (!stream_reader.Read ("TEXCOORD", set, "STP", tex_vertices, &TexVertex::coord))
      {
        LogError (surface_iter, "Error at read TEXCOORD stream from set '%s'", set);
        
        surfaces.Remove (surface);
        
        return;
      }
      
      if (!stream_reader.Read ("TEXTANGENT", set, "XYZ", tex_vertices, &TexVertex::tangent))
      {
        LogError (surface_iter, "Error at read TEXTANGENT stream from set '%s'", set);
        
        surfaces.Remove (surface);
        
        return;
      }
      
      if (!stream_reader.Read ("TEXBINORMAL", set, "XYZ", tex_vertices, &TexVertex::binormal))
      {
        LogError (surface_iter, "Error at read TEXBINORMAL stream from set '%s'", set);
        
        surfaces.Remove (surface);
        
        return;
      }
    }
    
    if (surface_info.inputs.FindChannel ("COLOR", set))
    {
      size_t channel = surface.ColorChannels ().Create (set);
      
      math::vec3f* colors = surface.ColorChannels ().Data (channel);
      
      if (!stream_reader.Read ("COLOR", set, "RGB", colors))
      {
        LogError (surface_iter, "Error at read COLOR stream from set '%s'", set);
        
        surfaces.Remove (surface);
        
        return;
      }      
    }
  }  
}
