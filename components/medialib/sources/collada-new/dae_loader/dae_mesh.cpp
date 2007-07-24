#include <stl/vector>
#include <stl/hash_map>
#include <stl/string>
#include <common/hash.h>

#include "shared.h"

using namespace medialib::collada;

/*
    ��������������� ��������� ������
*/

namespace medialib
{

namespace collada
{

typedef stl::vector<float> FloatBuffer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MeshSource
{
  FloatBuffer data;   //������
  stl::string params; //���������
  size_t      count;  //���������� ���������
  size_t      stride; //���
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������� ����� ��������� � ��������� �� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshSourceMap
{
  public:
      //�������� ��������� ������ ����
    MeshSource* Create (const char* name)
    {
      return &sources [name];
    }
    
      //�������� ��������� ������ ����
    void Remove (const char* name)
    {
      sources.erase (name);
    }
    
      //����� ��������� ������ ����
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
///����� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MeshInput
{
  Parser::Node* node;   //���� ������ �������
  MeshSource*   source; //�������� ������
  size_t        offset; //�������� � ������� ��������
  
  MeshInput () : node (0), source (0), offset (0) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ��������� ������ ������ ����
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
    
      //���������� ������ ������ ����
    bool AddChannel (const char* semantic, const char* set, Parser::Node* node, MeshSource* source, size_t offset)
    {
      if (!set)
        set = "";

      size_t                 hash = strhash (semantic, common::strhash (set));
      MeshInputMap::iterator iter = input_map.find (hash);
      
      if (iter != input_map.end ())
        return false; //���� ����� ��� ��� ���������������

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
    
      //����� ������
    const MeshInput* FindChannel (const char* semantic, const char* set=0)
    {
      if (!set)
        set = "";

      size_t                 hash = strhash (semantic, common::strhash (set));
      MeshInputMap::iterator iter = input_map.find (hash);

      return iter != input_map.end () ? &inputs [iter->second] : 0;
    }
    
      //��������� ���������� ������� ������
    size_t GetChannelsCount  () const { return inputs.size (); }
    
      //���������� �����������
    size_t GetSetsCount () const { return sets.size (); }
    
      //��������� ����� ������������
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
///��������� ����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshVertexBuffer
{
  public:
    MeshVertexBuffer (size_t in_inputs_count, size_t reserve_vertices_count)
    {
      inputs_count = in_inputs_count;
      
      vertices.reserve (reserve_vertices_count);
    }
    
      //���������� �������
    size_t AddVertex (size_t* inputs)
    {
        //����������� ���� ������� �� ������ �������� ������� ������� ������ (����� � ������ �������, ����� � ������ ��������, ...)

      size_t vertex_hash = GetVertexHash (inputs);

        //��������: ���������� �� ����� ������� � ������ ������

      VertexBufferMap::iterator search_iter = vertices_map.find (vertex_hash);

      if (search_iter != vertices_map.end ())
        return search_iter->second;

        //���� ������� ��� � ������ - ���������� � � ����� ������ � ����� ������

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
    
      //���������� ������ � ������
    size_t GetVerticesCount () const { return vertices.size (); }
    
      //��������� ������� ������
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

}

}

/*
    ������ ����
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
  
    //������ ���������� ������
    
  MeshSourceMap sources;
  
    //������ ������������
    
  for_each_child (iter, "source", bind (&DaeParser::ParseMeshSource, this, _1, ref (sources)));

//  for_each_child (iter, "polygons",   bind (&DaeParser::LogError, this, _1, "Polygons are not supported"));
//  for_each_child (iter, "polylist",   bind (&DaeParser::LogError, this, _1, "Polygons are not supported"));

  for_each_child (iter, "triangles", bind (&DaeParser::ParseSurface, this, _1, iter, ref (mesh), PrimitiveType_TriangleList, ref (sources)));
  
  
//  for_each_child (iter, "linestrips", bind (&DaeParser::ParseLineStrips, this, _1, ref (mesh)));
//  for_each_child (iter, "lines",      bind (&DaeParser::ParseLines, this, _1, ref (mesh)));
//  for_each_child (iter, "trifans",    bind (&DaeParser::ParseTriangleFans, this, _1, ref (mesh)));
//  for_each_child (iter, "tristrips",  bind (&DaeParser::ParseTriangleStrips, this, _1, ref (mesh)));
}

/*
    ������ ���������
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
    LogError (accessor_iter, "Wrong count/stride attribute. count * stride > float_array.count = %u", data_count);
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
    ������ ������������
*/

void DaeParser::ParseSurface
 (Parser::Iterator iter,
  Parser::Iterator mesh_iter,
  Mesh&            mesh,
  PrimitiveType    primitive_type,
  MeshSourceMap&   sources)
{
  LogScope scope (iter, *this);
  
    //��������� ����� ���������
    
  const char* material_name = get<const char*> (iter, "material");
  
  if (!material_name)
  {
    LogError (iter, "No 'material' attribute detected");
    return;
  }
  
  Material* material = model.Materials ().Find (material_name);
  
  if (!material)
  {
    LogError (iter, "Material '%s' not found", material_name);
    return;
  }  

    //�������� ������� ������ �����������

  MeshInputBuilder inputs;

  for_each_child (iter, "input", bind (&DaeParser::ParseSurfaceInput, this, _1, mesh_iter, ref (sources), ref (inputs)));
  
    //�������� ������� �����������
  
  if (!ParseSurfaceBuffers (iter, mesh, *material, primitive_type, inputs))
  {
    LogError (iter, "Error at build surface buffers");
    return;
  }
}

/*
    ������ ������� ������� ������ �����������
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
 
  source_name++; //����������� �� ����������� '#'
  
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
  else //���������� ������ ������
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
        
        source_name++; //����������� �� ����������� '#'
        
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
  
    //����������� ������ ������
    
  const char* set = get<const char*> (iter, "set");
    
  if (!inputs.AddChannel (semantic, set, iter, source, offset))
  {
    if (set) LogError (iter, "Input channel '%s' from set '%s' already registered. Will be ignored", semantic, set);
    else     LogError (iter, "Input channel '%s' already registered. Will be ignored", semantic);

    return;    
  }
}

namespace
{

//������ �������
template <size_t N>
void set_vertex_field (const float* src, math::vec<float, N>& res)
{
  for (size_t i=0; i<N; i++)
    res [i] = src [i];
}

//������ ������ ��������� ������
template <class T, class T1>
bool read_vertex_stream
 (DaeParser&        parser,
  Parser::Node*     surface_node,
  MeshInputBuilder& inputs,
  MeshVertexBuffer& vertex_buffer,
  const char*       semantic,
  const char*       set,
  const char*       params,
  T*                buffer,
  T1 T::*           field)
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
       
    set_vertex_field (source + index * stride, output_vertex->*field);
  }
  
  return true;
}

}

/*
    ���������� ������� �����������
      ������ ������� ������ ��������� � ��������� ������ ����������� �� ������ ������� ������� ������
*/

bool DaeParser::ParseSurfaceBuffers
 (Parser::Iterator  iter,
  Mesh&             mesh,
  Material&         material,
  PrimitiveType     primitive_type,
  MeshInputBuilder& inputs)
{  
    //����������� ��������� ������ ��������

  Parser::Iterator p_iter = iter->First ("p.#text");
  
  if (!p_iter)
  {
    LogError (iter, "No 'p' sub-tag detected");
    return false;
  }
  
  if (p_iter->Next ())
  {
    LogError (iter, "More than one 'p' sub-tag detected");
    return false;
  }
  
    //��������� ���������� ��������
  
  size_t indices_count = 0;
  
  if (!CheckedRead (iter, "count", indices_count))
  {
    LogError (iter, "Error at read 'count' attribute");
    return false;
  }  

    //������ ��������� ������ ��������

  typedef stl::vector<size_t> IndexBuffer;

  IndexBuffer input_indices;

  size_t inputs_count        = inputs.GetChannelsCount ();  
  size_t input_indices_count = indices_count * inputs_count;

  input_indices.resize (input_indices_count);  

  if (read_range (p_iter, 0, &input_indices [0], input_indices_count) != input_indices_count)
  {
    LogError (p_iter, "Wrong index buffer");
    return false;
  }
  
    //���������� ��������� ���������� ������ (������ ������� - ��������� �� ������ �������� �� ������� ������� ������)
    //�������������� ����������� ���������� ������� ������ ������ � ������ ��������    
  
  MeshVertexBuffer vertex_buffer (inputs_count, input_indices_count);

    //���������� ��������������� ������ �������� � �������� ����� ������
    
  IndexBuffer output_indices;
  
  output_indices.resize (indices_count);
    
  size_t* index = &output_indices [0];  

  for (IndexBuffer::iterator index_iter=input_indices.begin (), index_end=input_indices.end (); index_iter != index_end;
       index_iter += inputs_count, index++)
  {    
    *index = vertex_buffer.AddVertex (&*index_iter);
  }  
  
    //�������� �����������
    
  Surface& surface = mesh.Surfaces ().Create (material, primitive_type, vertex_buffer.GetVerticesCount (), indices_count);
  
    //����������� ������ ��������

  stl::copy (output_indices.begin (), output_indices.end (), surface.Indices ());
 
    //���������� ��������������� ������ ������
    
  Vertex* vertices = surface.Vertices ();

  if (!read_vertex_stream (*this, iter, inputs, vertex_buffer, "VERTEX", "", "XYZ", vertices, &Vertex::coord))
  {
    LogError (iter, "Error at read vertices stream");
    
    mesh.Surfaces ().Remove (surface);
    
    return false;
  }
  
  if (!read_vertex_stream (*this, iter, inputs, vertex_buffer, "NORMAL", "", "XYZ", vertices, &Vertex::normal))
  {
    LogError (iter, "Error at read normals stream");

    mesh.Surfaces ().Remove (surface);    

    return false;
  }
  
  return true;
}
