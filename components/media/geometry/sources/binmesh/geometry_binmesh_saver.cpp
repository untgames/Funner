#include "shared.h"

using namespace common;
using namespace media::geometry;

/*
 * array <> format: uint32_t elements_count; elements
 *
 * File format: int header; int version; data
 *
 * data format: array<vertex_stream>; array<vertex_weight_stream>; array<vertex_buffer_desc>; array<index_buffer>; array<mesh>
 *
 * vertex_stream format: uint32_t vertices_count; uint32_t vertex_size; array<vertex_stream_channel>; binary<data>
 * vertex_stream_channel format: VertexAttributeSemantic semantic; VertexAttributeType type; uint32_t offset
 * vertex_weight_stream format: array<VertexWeight>
 *
 * vertex_buffer_desc format: array<uint32_t> vertex_weigth_stream_id (0-1); array<uint32_t> vertex_stream_id;
 *
 * index_buffer format: IndexType type, array<index_type> indices
 *
 * mesh format: array<char> name; array<uint32_t> index_buffer (0-1); array<uint32_t> vertex_buffers; array<primitive> primitives;
 *
 * primitive format: PrimitiveType type; array<char> material; uint32_t vertex_buffer; uint32_t first; uint32_t count
 */

namespace components
{

namespace bin_mesh_saver
{

/*
    Константы
*/

const char HEADER [4] = {'B', 'M', 'S', 'H'};
const int VERSION = 3;

void file_write (OutputFile& file, const void* data, size_t size)
{
  if (file.Write (data, size) != size)
    throw xtl::format_operation_exception ("media::geometry::BinMeshLibrarySaver", "Can't write data to file");
}

void file_write (OutputFile& file, const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("media::geometry::BinMeshSaver", "string");

  unsigned int length = (unsigned int)xtl::xstrlen (string);

  file_write (file, &length, sizeof (length));
  file_write (file, string, length);
}

/*
    Класс, сохраняющий меш-модели в бинарном формате
*/

class BinMeshLibrarySaver
{
  private:
    typedef stl::hash_map<size_t, uint32_t> ResourceMap;

      //сохранение вершинного потока
    void SaveVertexStream (const VertexStream& vs)
    {
      ResourceMap::const_iterator iter = vertex_streams.find (vs.Id ());

      if (iter != vertex_streams.end ())
        return; //поток уже сохранён

        //сохранение заголовка потока

      const VertexFormat& vertex_format = vs.Format ();

      uint32_t vertices_count = vs.Size (),
               vertex_size    = vs.VertexSize ();

      file_write (result_file, &vertices_count, sizeof (vertices_count));
      file_write (result_file, &vertex_size, sizeof (vertex_size));

        //сохранение вершинных данных

      uint32_t channels_count = vertex_format.AttributesCount ();

      file_write (result_file, &channels_count, sizeof (channels_count));

      for (uint32_t i = 0; i < channels_count; i++)
      {
        const VertexAttribute& attribute = vertex_format.Attribute (i);

        file_write (result_file, &attribute.semantic, sizeof (attribute.semantic));
        file_write (result_file, &attribute.type,     sizeof (attribute.type));
        file_write (result_file, &attribute.offset,   sizeof (attribute.offset));
        file_write (result_file, attribute.name);
      }

      file_write (result_file, vs.Data (), vertices_count * vertex_size);

        //добавление потока в список сохранённых

      vertex_streams.insert_pair (vs.Id (), (uint32_t)vertex_streams.size ());
    }

      //сохранение потока вершинных весов

    void SaveVertexWeightStream (const VertexWeightStream& vws)
    {
        //сохранение заголовка потока

      uint32_t weights_count = vws.Size ();

      file_write (result_file, &weights_count, sizeof (weights_count));

        //сохранение весов

      file_write (result_file, vws.Data (), weights_count * sizeof (VertexWeight));

        //добавление потока в список сохранённых

      vertex_weights.insert_pair (vws.Id (), (uint32_t)vertex_weights.size ());
    }

      //сохранение вершинного буфера
    void SaveVertexBuffer (const VertexBuffer& vb)
    {
      if (!vb.VerticesCount ())
        return;

      ResourceMap::const_iterator iter = vertex_buffers.find (vb.Id ());

      if (iter != vertex_buffers.end ())
        return; //буфер уже сохранён

        //сохранение заголовка вершинного буфера

      ResourceMap::const_iterator weights_iter = vertex_weights.find (vb.Weights ().Id ());

      uint32_t weights_count = weights_iter != vertex_weights.end () ? 1 : 0;

      file_write (result_file, &weights_count, sizeof (weights_count));

      if (weights_count)
      {
        uint32_t weights_index = weights_iter->second;

        file_write (result_file, &weights_index, sizeof (weights_index));
      }

        //сохранение ссылок на вершинные потоки

      uint32_t vertex_streams_count = 0;

      for (uint32_t i = 0; i < vb.StreamsCount (); i++)
      {
        ResourceMap::const_iterator iter = vertex_streams.find (vb.Stream (i).Id ());

        if (iter != vertex_streams.end ())
          vertex_streams_count++;
      }

      file_write (result_file, &vertex_streams_count, sizeof (vertex_streams_count));

      for (uint32_t i = 0; i < vb.StreamsCount (); i++)
      {
        ResourceMap::const_iterator iter = vertex_streams.find (vb.Stream (i).Id ());

        if (iter == vertex_streams.end ())
          continue;

        uint32_t vertex_stream_index = iter->second;

        file_write (result_file, &vertex_stream_index, sizeof (vertex_stream_index));
      }

        //добавление буфера в список сохранённых

      vertex_buffers.insert_pair (vb.Id (), (uint32_t)vertex_buffers.size ());
    }

      //сохранение индексного буфера
    void SaveIndexBuffer (const IndexBuffer& ib)
    {
      if (!ib.Size ())
        return;

      ResourceMap::const_iterator iter = index_buffers.find (ib.Id ());

      if (iter != index_buffers.end ())
        return; //буфер уже сохранён

        //сохранение заголовка вершинного буфера

      IndexType data_type = ib.DataType ();

      file_write (result_file, &data_type, sizeof (data_type));

      uint32_t indices_count = ib.Size ();

      file_write (result_file, &indices_count, sizeof (indices_count));
      
        //сохранение индексов

      file_write (result_file, ib.Data (), get_index_type_size (data_type) * indices_count);

        //добавление потока в список сохранённых

      index_buffers.insert_pair (ib.Id (), (uint32_t)index_buffers.size ());
    }

      //сохранение вершинных буферов меша
    void SaveMeshVertexBuffers (const Mesh& mesh)
    {
      uint32_t vertex_buffers_count = 0;

      for (uint32_t i = 0, count = mesh.VertexBuffersCount (); i < count; i++)
      {
        ResourceMap::const_iterator vb_iter = vertex_buffers.find (mesh.VertexBuffer (i).Id ());

        if (vb_iter != vertex_buffers.end ())
          vertex_buffers_count++;
      }

      file_write (result_file, &vertex_buffers_count, sizeof (vertex_buffers_count));

      for (uint32_t i=0, count=mesh.VertexBuffersCount (); i<count; i++)
      {
        ResourceMap::const_iterator vb_iter = vertex_buffers.find (mesh.VertexBuffer (i).Id ());

        if (vb_iter != vertex_buffers.end ())
        {
          uint32_t vertex_buffer_index = vb_iter->second;

          file_write (result_file, &vertex_buffer_index, sizeof (vertex_buffer_index));
        }
      }
    }

      //сохранение примитива
    void SavePrimitive (const MaterialMap& material_map, const Primitive& primitive)
    {
      file_write (result_file, &primitive.type, sizeof (primitive.type));

      const char* material_name = material_map.MaterialName (primitive.material_id);

      uint32_t material_name_length = (uint32_t)xtl::xstrlen (material_name);

      file_write (result_file, &material_name_length, sizeof (material_name_length));
      file_write (result_file, material_name, sizeof (char) * material_name_length);

      file_write (result_file, &primitive.vertex_buffer, sizeof (primitive.vertex_buffer));
      file_write (result_file, &primitive.first, sizeof (primitive.first));
      file_write (result_file, &primitive.count, sizeof (primitive.count));
      file_write (result_file, &primitive.base_vertex, sizeof (primitive.base_vertex));
    }

      //сохранение примитивов
    void SaveMeshPrimitives (const Mesh& mesh)
    {
      uint32_t primitives_count = mesh.PrimitivesCount ();

      file_write (result_file, &primitives_count, sizeof (primitives_count));

      const MaterialMap& material_map = mesh.MaterialMap ();

      for (uint32_t i = 0; i < primitives_count; i++)
        SavePrimitive (material_map, mesh.Primitive (i));
    }

      //сохранение меша
    void SaveMesh (const char* id, const Mesh& mesh)
    {
      if (!mesh.VertexBuffersCount () || !mesh.PrimitivesCount ())
        return;        
        
      uint32_t id_length = (uint32_t)xtl::xstrlen (id);
      
      file_write (result_file, &id_length, sizeof (id_length));
      file_write (result_file, id, sizeof (char) * id_length);

      uint32_t name_length = (uint32_t)xtl::xstrlen (mesh.Name ());

      file_write (result_file, &name_length, sizeof (name_length));
      file_write (result_file, mesh.Name (), sizeof (char) * name_length);

      ResourceMap::const_iterator ib_iter = index_buffers.find (mesh.IndexBuffer ().Id ());

      uint32_t index_buffers_count = ib_iter != index_buffers.end () ? 1 : 0;

      file_write (result_file, &index_buffers_count, sizeof (index_buffers_count));

      if (index_buffers_count)
      {
        uint32_t index_buffer_index = ib_iter->second;

        file_write (result_file, &index_buffer_index, sizeof (index_buffer_index));
      }

      SaveMeshVertexBuffers (mesh);
      SaveMeshPrimitives (mesh);
    }

      //сохранение вершинных потоков
    void SaveVertexStreams ()
    {
        //запись количества потоков

      stl::set<size_t> vertex_streams_set;

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        for (uint32_t j = 0, count = i->VertexBuffersCount (); j < count; j++)
          for (uint32_t k = 0, vertex_buffer_streams_count = i->VertexBuffer (j).StreamsCount (); k < vertex_buffer_streams_count; k++)
            vertex_streams_set.insert (i->VertexBuffer (j).Stream (k).Id ());

      uint32_t vertex_streams_count = (uint32_t)vertex_streams_set.size ();

      file_write (result_file, &vertex_streams_count, sizeof (vertex_streams_count));

        //запись потоков

      for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
      {
        const Mesh& mesh = *i;

        for (uint32_t k=0, count=mesh.VertexBuffersCount (); k<count; k++)
        {
          const VertexBuffer& vb = mesh.VertexBuffer (k);

          for (uint32_t j = 0, vertex_buffer_streams_count = vb.StreamsCount (); j < vertex_buffer_streams_count; j++)
            SaveVertexStream (vb.Stream (j));
        }
      }
    }

      //сохранение весов вершин
    void SaveVertexWeights ()
    {
        //запись количества потоков

      stl::set<size_t> weights_streams_set;

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        for (uint32_t k = 0, count = i->VertexBuffersCount (); k < count; k++)
        {
          const VertexWeightStream& vws = i->VertexBuffer (k).Weights ();

          if (vws.Size ())
            weights_streams_set.insert (vws.Id ());
        }

      uint32_t weights_streams_count = (uint32_t)weights_streams_set.size ();

      file_write (result_file, &weights_streams_count, sizeof (weights_streams_count));

        //запись потоков

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        for (uint32_t k = 0, count = i->VertexBuffersCount (); k<count; k++)
        {
          const VertexWeightStream& vws = i->VertexBuffer (k).Weights ();

          if (vws.Size ())
          {
            ResourceMap::const_iterator iter = vertex_weights.find (vws.Id ());

            if (iter == vertex_weights.end ())
              SaveVertexWeightStream (vws);
          }
        }
    }

      //сохранение вершинных буферов
    void SaveVertexBuffers ()
    {
      stl::set<size_t> vertex_buffers_set;

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
      {
        for (uint32_t j = 0, count = i->VertexBuffersCount (); j < count; j++)
        {
          const VertexBuffer& vb = i->VertexBuffer (j);

          if (vb.VerticesCount ())
            vertex_buffers_set.insert (vb.Id ());
        }
      }

      uint32_t vertex_buffers_count = (uint32_t)vertex_buffers_set.size ();

      file_write (result_file, &vertex_buffers_count, sizeof (vertex_buffers_count));

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        for (uint32_t j = 0, count = i->VertexBuffersCount (); j < count; j++)
          SaveVertexBuffer (i->VertexBuffer (j));
    }

      //сохранение индексных буферов
    void SaveIndexBuffers ()
    {
      stl::set<size_t> index_buffers_set;

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
      {
        const IndexBuffer& ib = i->IndexBuffer ();

        if (ib.Size ())
          index_buffers_set.insert (ib.Id ());
      }

      uint32_t index_buffers_count = (uint32_t)index_buffers_set.size ();

      file_write (result_file, &index_buffers_count, sizeof (index_buffers_count));

      for (MeshLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
        SaveIndexBuffer (i->IndexBuffer ());
    }

      //сохранение мешей
    void SaveMeshes ()
    {
      uint32_t meshes_count = 0;

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        if (i->VertexBuffersCount () && i->PrimitivesCount ())
          meshes_count++;

      file_write (result_file, &meshes_count, sizeof (meshes_count));

      for (MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        SaveMesh (library.ItemId (i), *i);
    }

    void SaveHeader ()
    {
      file_write (result_file, HEADER, sizeof (HEADER));
      file_write (result_file, &VERSION, sizeof (VERSION));
    }

      //сохранение библиотеки
    void SaveLibrary ()
    {
      SaveHeader ();

      SaveVertexStreams ();
      SaveVertexWeights ();
      SaveVertexBuffers ();
      SaveIndexBuffers  ();
      SaveMeshes        ();
    }

  public:
      //конструктор
    BinMeshLibrarySaver (const char* file_name, const MeshLibrary& in_library) : result_file (file_name), library (in_library)
    {
      SaveLibrary ();
    }

  private:
    OutputFile         result_file;     //результирующий файл
    const MeshLibrary& library;         //сохраняемая библиотека
    ResourceMap        vertex_streams;  //сохранённые вершинные потоки
    ResourceMap        vertex_buffers;  //сохранённые вершинные буферы
    ResourceMap        vertex_weights;  //сохранённые потоки вершинных весов
    ResourceMap        index_buffers;   //сохранённые индексные буферы
};

/*
    Автоматическая регистрация компонента
*/

class BinMeshSaverComponent
{
  public:
    BinMeshSaverComponent ()
    {
      MeshLibraryManager::RegisterSaver ("binmesh", &SaveLibrary);
    }

  private:
    static void SaveLibrary (const char* file_name, const MeshLibrary& library)
    {
      BinMeshLibrarySaver (file_name, library);
    }
};

extern "C"
{

ComponentRegistrator<BinMeshSaverComponent> BinMeshSaver ("media.geometry.savers.BinMeshSaver");

}

}

}
