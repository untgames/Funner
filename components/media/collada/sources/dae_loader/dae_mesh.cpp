#include "shared.h"

using namespace media::collada;

/*
    Вспомогательные структуры данных
*/

namespace media
{

namespace collada
{

/*
    Класс для безопасного чтения NaN
*/

#ifdef _MSC_VER
  #pragma pack (push)
  #pragma pack (1)
#else
  #pragma pack (1)
#endif

struct safe_float { float value; };

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

bool read (xtl::io::token_iterator<const char*>& iter, safe_float& result_value)
{
  if (read (iter, result_value.value))
  {
    if (result_value.value != result_value.value)
      result_value.value = 0.f;

    return true;
  }

  if (!iter)
    return false;

  if (!xtl::xstricmp (*iter, "NAN"))
  {
    ++iter;

    result_value.value = 0.0;

    return true;
  }

  return false;
}

typedef stl::vector<float> FloatBuffer;

/*
    Источник данных меша
*/

struct MeshSource
{
  FloatBuffer  data;   //данные
  stl::string  params; //параметры
  unsigned int count;  //количество элементов
  unsigned int stride; //шаг
};

/*
    Карта отображения имени источника в указатель на него
*/

class MeshSourceMap
{
  public:
///Создание источника данных меша
    MeshSource* Create (const char* name) { return &sources [name]; }

///Удаление источника данных меша
    void Remove (const char* name) { sources.erase (name); }
    
///Поиск источника данных меша
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

/*
    Канал данных меша
*/

struct MeshInput
{
  ParseNode    node;   //узел дерева разбора
  MeshSource*  source; //источник данных
  unsigned int offset; //смещение в массиве индексов

  MeshInput (const ParseNode& in_node, MeshSource* in_source, unsigned int in_offset) :
    node (in_node), source (in_source), offset (in_offset) {}
};

/*
    Класс, создающий каналы данных меша
*/

class MeshInputBuilder
{
  enum
  {
    DEFAULT_INPUTS_RESERVE = 32,
    DEFAULT_SETS_RESERVE   = 8
  };
  
  public:   
///Конструктор
    MeshInputBuilder ()
      : max_offset (0)
    {       
      inputs.reserve (DEFAULT_INPUTS_RESERVE);
      sets.reserve (DEFAULT_SETS_RESERVE);
    }
    
///Добавление канала данных меша
    bool AddChannel (const char* semantic, unsigned int set, const ParseNode& node, MeshSource* source, unsigned int offset)
    {
      size_t                 hash = strhash (semantic, set);
      MeshInputMap::iterator iter = input_map.find (hash);

      if (iter != input_map.end ())
        return false; //этот канал уже был зарегистрирован

      inputs.push_back (MeshInput (node, source, offset));

      if (offset > max_offset)
        max_offset = offset;

      try
      {       
        input_map [hash] = (unsigned int)inputs.size () - 1;
        
        RegisterSet (set);
          
        return true;
      }
      catch (...)
      {
        inputs.pop_back ();
        throw;
      }      
    }
    
///Поиск канала
    const MeshInput* FindChannel (const char* semantic, unsigned int set=0)
    {
      size_t                 hash = strhash (semantic, set);
      MeshInputMap::iterator iter = input_map.find (hash);

      return iter != input_map.end () ? &inputs [iter->second] : 0;
    }
    
///Получение количества каналов данных
    unsigned int GetChannelsCount  () const { return (unsigned int)inputs.size (); }
    
///Количество подмножеств
    unsigned int GetSetsCount () const { return (unsigned int)sets.size (); }
    
///Максимальное смещение канала
    unsigned int GetMaxOffset () const { return max_offset; }

///Получение значения подмножества
    unsigned int GetSetValue (unsigned int index) const { return sets [index]; }

  private:
    void RegisterSet (unsigned int set)
    {
      for (SetArray::iterator i=sets.begin (), end=sets.end (); i!=end; ++i)
        if (*i == set)
          return;
          
      sets.push_back (set);
    }

  private:
    typedef stl::hash_map<size_t, unsigned int> MeshInputMap;
    typedef stl::vector<MeshInput>              MeshInputArray;
    typedef stl::vector<unsigned int>           SetArray;
    
  private:
    MeshInputArray inputs;
    MeshInputMap   input_map;
    SetArray       sets;
    unsigned int   max_offset;
};

/*
    Вершинный буфер меша
*/

struct VertexKey
{
  unsigned int* inputs;
  unsigned int  inputs_count;
  
  VertexKey (unsigned int* in_inputs, unsigned int in_inputs_count) : inputs (in_inputs), inputs_count (in_inputs_count) {}
  
  bool operator == (const VertexKey& key) const { return inputs_count == key.inputs_count && memcmp (inputs, key.inputs, inputs_count * sizeof (unsigned int)) == 0; }
  bool operator != (const VertexKey& key) const { return !(*this == key); }
};

size_t hash (const VertexKey& key)
{
  return common::crc32 (key.inputs, sizeof (unsigned int) * key.inputs_count);
}

class MeshVertexBuffer
{
  public:
///Конструктор
    MeshVertexBuffer (unsigned int in_inputs_count, unsigned int reserve_vertices_count)
    {
      inputs_count = in_inputs_count;
      
      vertices.reserve (reserve_vertices_count);
    }
    
///Добавление вершины
    unsigned int AddVertex (unsigned int* inputs)
    {
      VertexKey key (inputs, inputs_count);
      
        //проверка: существует ли такая вершина в буфере вершин

      VertexBufferMap::iterator search_iter = vertices_map.find (key);

      if (search_iter != vertices_map.end ())
        return search_iter->second;

        //если вершины нет в буфере - добавление её в буфер вершин и карту вершин

      unsigned int vertex_index = (unsigned int)vertices.size ();

      vertices.push_back (inputs);

      try
      {
        vertices_map [key] = vertex_index;

        return vertex_index;
      }
      catch (...)
      {
        vertices.pop_back ();
        throw;
      }      
    }
    
///Количество вершин в буфере
    unsigned int GetVerticesCount () const { return (unsigned int)vertices.size (); }
    
///Получение массива вершин
    unsigned int** GetVertices () { return &vertices [0]; }

  private:
    typedef stl::hash_map<VertexKey, unsigned int> VertexBufferMap;
    typedef stl::vector<unsigned int*>             VertexBuffer;
    
    VertexBuffer    vertices;
    VertexBufferMap vertices_map;
    unsigned int    inputs_count;
};

/*
    Информация для создания поверхности
*/

struct SurfaceInfo
{
  Mesh*            mesh;
  const char*      material_name;
  PrimitiveType    primitive_type;
  MeshInputBuilder inputs;
};

/*
    Класс упрощающий чтение каналов вершинных данных
*/

class VertexStreamReader
{
  public:
///Конструктор
    VertexStreamReader (const ParseNode&  in_surface_node,
                        MeshInputBuilder& in_inputs,
                        MeshVertexBuffer& in_vertex_buffer)
       : surface_node (in_surface_node),
         inputs (in_inputs),
         vertex_buffer (in_vertex_buffer) {}

///Чтение канала данных в поле, определеяемое указателем на член-класса
    template <class T, class Field>
    void Read (const char* semantic, unsigned int set, const char* params, T* buffer, Field T::* field)
    {
      ReadCore (semantic, set, params, buffer, field_selector<T, Field> (field));
    }

///Непосредственное чтение канала данных
    template <class T>
    void Read (const char* semantic, unsigned int set, const char* params, T* buffer)
    {
      ReadCore (semantic, set, params, buffer, identity_selector ());
    }    
    
///Чтение массива индексов вершин
    void ReadVertexIndices (unsigned int* buffer)
    {
      const MeshInput* input = inputs.FindChannel ("VERTEX");

      if (!input)
        raise_parser_exception (surface_node, "No input channel with semantic='VERTEX'");

      unsigned int offset         = input->offset,
                   max_count      = input->source->count,
                   vertices_count = vertex_buffer.GetVerticesCount ();

      if (offset >= inputs.GetChannelsCount ())
        raise_parser_exception (input->node, "Offset %u is greater of inputs count %u", offset, inputs.GetChannelsCount ());

      unsigned int **input_vertex = vertex_buffer.GetVertices (), *output_index = buffer;

      for (unsigned int i=0; i<vertices_count; i++, input_vertex++, output_index++)
      {
        unsigned int index = (*input_vertex) [offset];
        
        if (index >= max_count)
          raise_parser_exception (surface_node.First ("p"), "Wrong index %u (max_count=%u)", index, max_count);

        *output_index = index;
      }
    }
    
  private:
///Функтор возвращающий член класса
    template <class T, class Field> struct field_selector
    {
      field_selector (Field T::* in_field) : field (in_field) {}
      
      Field& operator () (T& object) const { return object.*field; }
      
      Field T::* field;
    };
    
///Функтор возвращающий сам элемент
    struct identity_selector
    {
      template <class T> T& operator () (T& object) const { return object; }
    };
  
      //чтение канала данных
    template <class T, class Fn>  
    void ReadCore (const char* semantic, unsigned int set, const char* params, T* buffer, Fn fn)
    {
      const MeshInput* input = inputs.FindChannel (semantic, set);
      
      if (!input)
        raise_parser_exception (surface_node, "No input channel with semantic='%s' and set='%u'", semantic, set);
      
      if (input->source->params != params)
        if (xtl::xstrcmp (params, "STP") || input->source->params != "ST")
          raise_parser_exception (input->node, "Wrong params '%s'. Must be '%s'", input->source->params.c_str (), params);

      const float* source         = &input->source->data [0];
      unsigned int max_count      = input->source->count,
                   offset         = input->offset,
                   stride         = input->source->stride,
                   vertices_count = vertex_buffer.GetVerticesCount ();
                   
      if (offset >= inputs.GetChannelsCount ())
        raise_parser_exception (input->node, "Offset %u is greater of inputs count %u", offset, inputs.GetChannelsCount ());

      unsigned int** input_vertex  = vertex_buffer.GetVertices ();
      T*       output_vertex = buffer;  

      for (unsigned int i=0; i<vertices_count; i++, input_vertex++, output_vertex++)
      {
        unsigned int index = (*input_vertex) [offset];
        
        if (index >= max_count)
          raise_parser_exception (surface_node.First ("p"), "Wrong index %u (max_count=%u)", index, max_count);
           
        SetField (source + index * stride, fn (*output_vertex));
      }
    }  
    
  private:
///Чтение вектора
    template <unsigned int N>
    void SetField (const float* src, math::vector<float, N>& res)
    {
      for (unsigned int i=0; i<N; i++)
        res [i] = src [i];
    }
  
  private:
    ParseNode         surface_node;
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
  if (!iter->First ("vertices"))
    raise_parser_exception (*iter, "No vertices information. Node 'vertices' not found");

  if (!iter->First ("vertices.input"))
    raise_parser_exception (*iter, "Incorrect vertices information, 'input' URL not found");

    //чтение источников данных

  MeshSourceMap sources;

  for_each_child (*iter, "source", bind (&DaeParser::ParseMeshSource, this, _1, ref (sources)));

    //чтение поверхностей

  for_each_child (*iter, "polygons",  bind (&DaeParser::ParsePolygonSurface, this, _1));
  for_each_child (*iter, "polylist",  bind (&DaeParser::ParsePolygonSurface, this, _1));
  for_each_child (*iter, "triangles", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleList, ref (sources)));
  for_each_child (*iter, "tristrips", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleStrip, ref (sources)));
  for_each_child (*iter, "trifans",   bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleFan, ref (sources)));
  for_each_child (*iter, "lines",     bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_LineList, ref (sources)));
  for_each_child (*iter, "linestrip", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_LineStrip, ref (sources)));
}

/*
    Разбор источника
*/

void DaeParser::ParseMeshSource (Parser::Iterator iter, MeshSourceMap& sources)
{
  try
  {
    const char* volatile id = get<const char*> (*iter, "id");

    Parser::Iterator accessor_iter = get_first_child (*iter, "technique_common.accessor");

    unsigned int data_count = get<unsigned int> (*iter, "float_array.count"),
                 count      = get<unsigned int> (*accessor_iter, "count"),
                 stride     = get<unsigned int> (*accessor_iter, "stride");

    if (data_count < count * stride)
      raise_parser_exception (*accessor_iter, "Wrong count/stride attribute. count * stride > float_array.count. count=%u, stride=%u, "
        "float_array.count=%u", count, stride, data_count);

    MeshSource* source = sources.Create (id);  

    try
    {
      source->count  = count;
      source->stride = stride;    

      source->data.resize (data_count);    

      read (*iter, "float_array.#text", reinterpret_cast<safe_float*> (&source->data [0]), data_count);

      for (Parser::NamesakeIterator param_iter=accessor_iter->First ("param"); param_iter; ++param_iter)
        source->params += get<const char*> (*param_iter, "name");
    }
    catch (...)
    {
      sources.Remove (id);
      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::collada::DaeParser::ParseMeshSource");
    throw;
  }
}

/*
    Разбор поверхностей
*/

void DaeParser::ParsePolygonSurface (Parser::Iterator iter)
{
  raise_parser_exception (*iter, "Polygons not supported");
}

void DaeParser::ParseSurface
 (Parser::Iterator iter,
  Parser::Iterator mesh_iter,
  Mesh&            mesh,
  PrimitiveType    primitive_type,
  MeshSourceMap&   sources)
{
  SurfaceInfo surface_info;
  
  surface_info.mesh           = &mesh;
  surface_info.primitive_type = primitive_type;
  
    //получение имени материала
    
  surface_info.material_name = get<const char*> (*iter, "material");

    //загрузка каналов данных поверхности

  for_each_child (*iter, "input", bind (&DaeParser::ParseSurfaceInput, this, _1, mesh_iter, ref (sources), ref (surface_info.inputs)));
  
    //загрузка буферов поверхности

  for_each_child (*iter, "p", bind (&DaeParser::ParseSurfaceBuffers, this, _1, iter, ref (surface_info)));
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
  const char *semantic    = get<const char*> (*iter, "semantic"),
             *source_name = get<const char*> (*iter, "source");

  unsigned int offset = get<unsigned int> (*iter, "offset");

  source_name++; //избавляемся от префиксного '#'

  MeshSource* source = 0;

  if (::strcmp (semantic, "VERTEX"))
  {
    source = sources.Find (source_name);

    if (!source)
      raise_parser_exception (*iter, "No 'source' tag with id='%s' referenced in surface input node", source_name);
  }
  else //добавление канала вершин
  {
    for (Parser::NamesakeIterator verts_iter=mesh_iter->First ("vertices"); verts_iter; ++verts_iter)
    {
      const char* id = get<const char*> (*verts_iter, "id", "");
      
      if (!::strcmp (id, source_name))
      {
        const char *semantic    = get<const char*> (*verts_iter, "input.semantic"),
                   *source_name = get<const char*> (*verts_iter, "input.source");

        if (::strcmp (semantic, "POSITION"))
          raise_parser_exception (*verts_iter, "Wrong semantic '%s' in vertices input node. Must be 'POSITION'", semantic);

        source_name++; //избавляемся от префиксного '#'
        
        source = sources.Find (source_name);

        if (!source)
          raise_parser_exception (*verts_iter, "No 'input.source' tag with id='%s' referenced in vertices input node", source_name);
      }
    }

    if (!source)
      raise_parser_exception (*iter, "No 'vertices' tag with id='%s' referenced in input surface node", source_name);
  }
  
    //регистрация канала данных

  unsigned int set = get<unsigned int> (*iter, "set", 0);
    
  if (!inputs.AddChannel (semantic, set, *iter, source, offset))
    raise_parser_exception (*iter, "Input channel '%s' from set '%u' already registered. Will be ignored", semantic, set);
}

/*
    Построение буферов поверхности
      Данная функция строит вершинный и индексный буферы поверхности на основе входных каналов данных
*/

void DaeParser::ParseSurfaceBuffers (Parser::Iterator p_iter, Parser::Iterator surface_iter, SurfaceInfo& surface_info)
{  
    //получение количества индексов

  unsigned int indices_count = get<unsigned int> (*surface_iter, "count");

    //преобразование количества индексов

  switch (surface_info.primitive_type)
  {
    case PrimitiveType_TriangleList:
      indices_count *= 3;
      break;
    case PrimitiveType_TriangleStrip:
    case PrimitiveType_TriangleFan:
      indices_count += 2;
      break;
    case PrimitiveType_LineList:
      indices_count *= 2;
      break;
    case PrimitiveType_LineStrip:
      indices_count += 1;
      break;
    default:
      raise_parser_exception (*surface_iter, "Internal error: invalid primitive_type=%d", surface_info.primitive_type);
  }  

    //чтение исходного буфера индексов

  typedef stl::vector<unsigned int> IndexBuffer;

  IndexBuffer input_indices;

  unsigned int inputs_count        = surface_info.inputs.GetMaxOffset () + 1,
               input_indices_count = indices_count * inputs_count;

  input_indices.resize (input_indices_count);

  read (*p_iter, "#text", &input_indices [0], input_indices_count);

    //построение исходного вершинного буфера (каждый элемент - указатель на массив индексов во входных каналах данных)
    //резервирование максимально возможного размера буфера вершин и буфера индексов

  MeshVertexBuffer vertex_buffer (inputs_count, input_indices_count);

    //построение результирующего буфера индексов и исходный буфер вершин

  IndexBuffer output_indices;
  
  output_indices.resize (indices_count);
    
  unsigned int* index = &output_indices [0];

  for (IndexBuffer::iterator index_iter=input_indices.begin (), index_end=input_indices.end (); index_iter != index_end;
       index_iter += inputs_count, index++)
  {
    *index = vertex_buffer.AddVertex (&*index_iter);
  }  

    //создание поверхности
    
  Surface surface (surface_info.primitive_type, vertex_buffer.GetVerticesCount (), indices_count);
  
    //установка имени материала

  surface.SetMaterial (surface_info.material_name);

    //копирование буфера индексов

  stl::copy (output_indices.begin (), output_indices.end (), surface.Indices ());

    //создание объекта, читающего каналы вершинных данных

  VertexStreamReader stream_reader (*surface_iter, surface_info.inputs, vertex_buffer);
 
    //построение результирующего буфера вершин
    
  Vertex* vertices = surface.Vertices ();
  
  stream_reader.Read ("VERTEX", 0, "XYZ", vertices, &Vertex::coord);
  stream_reader.Read ("NORMAL", 0, "XYZ", vertices, &Vertex::normal);

    //построение каналов текстурных координат
    
  for (unsigned int i=0; i<surface_info.inputs.GetSetsCount (); i++)
  {
    unsigned int set = surface_info.inputs.GetSetValue (i);
    
    if (surface_info.inputs.FindChannel ("TEXCOORD", set))
    {    
      unsigned int channel_set = set;

      if (common::wcmatch (authoring_tool.c_str (), "OpenCOLLADA for 3ds Max*"))
        channel_set++;

      unsigned int channel = surface.TexVertexChannels ().Create (channel_set);

      TexVertex* tex_vertices = surface.TexVertexChannels ().Data (channel);      

      stream_reader.Read ("TEXCOORD", set, "STP", tex_vertices, &TexVertex::coord);
      stream_reader.Read ("TEXTANGENT", channel_set, "XYZ", tex_vertices, &TexVertex::tangent);
      stream_reader.Read ("TEXBINORMAL", channel_set, "XYZ", tex_vertices, &TexVertex::binormal);
    }

    if (surface_info.inputs.FindChannel ("COLOR", set))
    {
      unsigned int channel = surface.ColorChannels ().Create (set);
      
      math::vec3f* colors = surface.ColorChannels ().Data (channel);
      
      stream_reader.Read ("COLOR", set, "RGB", colors);
    }
  }
  
    //построение массива вершинных индексов
    
  VertexIndexMapPtr vertex_index_map = GetVertexIndicesMap (surface_info.mesh->Id (), surface_info.mesh->Surfaces ().Size ());
  
  vertex_index_map->Resize (surface.VerticesCount ());

  stream_reader.ReadVertexIndices (vertex_index_map->Indices ());

    //присоединение поверхности к мешу

  surface_info.mesh->Surfaces ().Add (surface);
}
