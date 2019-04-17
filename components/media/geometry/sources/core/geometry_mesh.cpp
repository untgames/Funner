#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace stl;
using namespace common;

const size_t DEFAULT_PRIMITIVES_ARRAY_RESERVE = 8; //количество резервируемых примитивов
const size_t DEFAULT_VB_ARRAY_RESERVE         = 4; //количество резервируемых вершинных буферов

/*
    Описание реализации Mesh
*/

typedef stl::vector<Primitive>    PrimitiveArray;
typedef stl::vector<VertexBuffer> VertexBufferArray;

struct Mesh::Impl: public xtl::reference_counter, public xtl::trackable
{
  object_id_t                  id;                           //идентификатор меша
  object_id_t                  source_id;                    //идентификатор меша из которого был клонирован / десериализован данный меш
  string                       name;                         //имя меша
  media::geometry::MaterialMap material_map;                 //карта материалов
  VertexBufferArray            vertex_buffers;               //вершинные буферы
  media::geometry::IndexBuffer index_buffer;                 //индексный буфер
  PrimitiveArray               primitives;                   //примитивы
  unsigned int                 structure_update_index;       //индекс обновления структуры меша (индекс буфер был заменен, либо вершинный буфер был добавлен или удален)
  unsigned int                 primitives_data_update_index; //индекс обновления данных примитивов
  
  Impl (); 
  Impl (const Impl&); //used for clone
};

/*
    Impl::Impl
*/

Mesh::Impl::Impl ()
  : id (IdPool::AllocateId (ObjectType_Mesh))
  , source_id (id)
  , structure_update_index (0)
  , primitives_data_update_index (0)
{
  primitives.reserve (DEFAULT_PRIMITIVES_ARRAY_RESERVE);
  vertex_buffers.reserve (DEFAULT_VB_ARRAY_RESERVE);
}

Mesh::Impl::Impl (const Impl& impl)
  : id (IdPool::AllocateId (ObjectType_Mesh))
  , source_id (impl.source_id)
  , name (impl.name)
  , material_map (impl.material_map.Clone ())
  , index_buffer (impl.index_buffer.Clone ())
  , primitives (impl.primitives)
  , structure_update_index (0)
  , primitives_data_update_index (0)
{
  vertex_buffers.reserve (impl.vertex_buffers.size ());

  for (VertexBufferArray::const_iterator iter = impl.vertex_buffers.begin (), end = impl.vertex_buffers.end (); iter != end; ++iter)
    vertex_buffers.push_back (iter->Clone ());
}

/*
    Конструкторы / деструктор
*/

Mesh::Mesh ()
  : impl (new Impl, false)
  {}
  
Mesh::Mesh (Impl* in_impl)
  : impl (in_impl, false)
  {}

Mesh::Mesh (const Mesh& mesh)
  : impl (mesh.impl)
  {}

Mesh::~Mesh ()
{
}

/*
    Присваивание
*/

Mesh& Mesh::operator = (const Mesh& mesh)
{
  impl = mesh.impl;

  return *this;
}

/*
    Создание копии
*/

Mesh Mesh::Clone () const
{
  return Mesh (new Impl (*impl));
}

/*
    Идентификатор меша
*/

object_id_t Mesh::Id () const
{
  return impl->id;
}

/*
   Идентификатор меша из которого был клонирован / десериализован данный меш
*/

object_id_t Mesh::SourceId () const
{
  return impl->source_id;
}

/*
    Имя меша
*/

const char* Mesh::Name () const
{
  return impl->name.c_str ();
}

void Mesh::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::geometry::Mesh::Rename", "name");
    
  impl->name = name;
}

/*
   Карта материалов
*/

void Mesh::Attach (const geometry::MaterialMap& map)
{
  impl->material_map = map;
}

void Mesh::DetachMaterialMap ()
{
  impl->material_map = geometry::MaterialMap ();
}

const geometry::MaterialMap& Mesh::MaterialMap () const
{
  return impl->material_map;
}

geometry::MaterialMap& Mesh::MaterialMap ()
{
  return impl->material_map;
}

/*
    Буферы
*/

//количество вершинных буферов
uint32_t Mesh::VertexBuffersCount () const
{
  return (uint32_t)impl->vertex_buffers.size ();
}

const media::geometry::VertexBuffer& Mesh::VertexBuffer (uint32_t index) const
{
  if (index >= impl->vertex_buffers.size ())
    throw xtl::make_range_exception ("media::geometry::Mesh::VertexBuffer", "index", index, impl->vertex_buffers.size ());

  return impl->vertex_buffers [index];
}

media::geometry::VertexBuffer& Mesh::VertexBuffer (uint32_t index)
{
  return const_cast<media::geometry::VertexBuffer&> (const_cast<const Mesh&> (*this).VertexBuffer (index));
}

const media::geometry::IndexBuffer& Mesh::IndexBuffer () const
{
  return impl->index_buffer;
}

media::geometry::IndexBuffer& Mesh::IndexBuffer ()
{
  return impl->index_buffer;
}


/*
    Присоединение/отсоединение буферов
*/

uint32_t Mesh::Attach (const media::geometry::VertexBuffer& vb)
{
  if (impl->vertex_buffers.size () == (uint32_t)-1)
    throw xtl::format_operation_exception ("media::geometry::Mesh::Attach", "Vertex buffers array max size exceeded");

  impl->vertex_buffers.push_back (vb);

  impl->structure_update_index++;

  return (uint32_t)impl->vertex_buffers.size () - 1;
}

void Mesh::Attach (const media::geometry::IndexBuffer& ib)
{
  impl->index_buffer = ib;

  impl->structure_update_index++;
}

void Mesh::DetachVertexBuffer (uint32_t index)
{
  if (index >= impl->vertex_buffers.size ())
    return;

  impl->vertex_buffers.erase (impl->vertex_buffers.begin () + index);

  impl->structure_update_index++;
}

void Mesh::DetachIndexBuffer ()
{
  impl->index_buffer = media::geometry::IndexBuffer ();

  impl->structure_update_index++;
}

void Mesh::DetachAllVertexBuffers ()
{
  impl->vertex_buffers.clear ();

  impl->structure_update_index++;
}

void Mesh::DetachAllBuffers ()
{
  DetachAllVertexBuffers ();
  DetachIndexBuffer ();
}

/*
    Количество примитивов / доступ к примитивам
*/

uint32_t Mesh::PrimitivesCount () const
{
  return (uint32_t)impl->primitives.size ();
}

const Primitive& Mesh::Primitive (uint32_t index) const
{
  if (index >= impl->primitives.size ())
    throw xtl::make_range_exception ("media::geometry::Mesh::Primitive", "index", index, impl->primitives.size ());
    
  return impl->primitives [index];
}

/*
    Добавление/удаление примитивов примитивов
*/

uint32_t Mesh::AddPrimitive (PrimitiveType type, uint32_t vertex_buffer, uint32_t first, uint32_t count, uint32_t base_vertex, const char* material)
{
  if (!material)
    material = "";

  int32_t material_id = impl->material_map.FindMaterialId (material);

  if (material_id == MaterialMap::INVALID_MATERIAL_ID)
    material_id = impl->material_map.SetMaterial (material);

  return AddPrimitive (type, vertex_buffer, first, count, base_vertex, material_id);
}

uint32_t Mesh::AddPrimitive (PrimitiveType type, uint32_t vertex_buffer, uint32_t first, uint32_t count, uint32_t base_vertex, uint32_t material_id)
{
  static const char* METHOD_NAME = "media::geometry::Mesh::AddPrimitive";

  if (type < 0 || type >= PrimitiveType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);
    
  if (impl->primitives.size () == (uint32_t)-1)
    throw xtl::format_operation_exception (METHOD_NAME, "Primitives max count exceeded");

  media::geometry::Primitive primitive;
  
  primitive.type          = type;
  primitive.vertex_buffer = vertex_buffer;
  primitive.first         = first;
  primitive.count         = count;
  primitive.base_vertex   = base_vertex;
  primitive.material_id   = material_id;

  impl->primitives.push_back (primitive);

  impl->primitives_data_update_index++;

  return (uint32_t)impl->primitives.size () - 1;
}

uint32_t Mesh::AddPrimitive (PrimitiveType type, uint32_t vertex_buffer, uint32_t first, uint32_t count, const char* material)
{
  return AddPrimitive (type, vertex_buffer, first, count, 0, material);
}

uint32_t Mesh::AddPrimitive (PrimitiveType type, uint32_t vertex_buffer, uint32_t first, uint32_t count, uint32_t material_id)
{
  return AddPrimitive (type, vertex_buffer, first, count, 0, material_id);
}

void Mesh::RemovePrimitive (uint32_t primitive_index)
{
  if (primitive_index >= impl->primitives.size ())
    return;

  impl->primitives.erase (impl->primitives.begin () + primitive_index);

  impl->primitives_data_update_index++;
}

void Mesh::RemoveAllPrimitives ()
{
  impl->primitives.clear ();

  impl->primitives_data_update_index++;
}

/*
    Очистка меша
*/

void Mesh::Clear ()
{
  RemoveAllPrimitives ();
  DetachAllBuffers ();
}

/*
   Текущий индекс обновления
*/

unsigned int Mesh::CurrentStructureUpdateIndex () const
{
  return impl->structure_update_index;
}

unsigned int Mesh::CurrentPrimitivesDataUpdateIndex () const
{
  return impl->primitives_data_update_index;
}

/*
   Сериализация / десериализация
*/

size_t Mesh::SerializationSize () const
{
  uint32_t vertex_buffers_size = 0;

  for (VertexBufferArray::iterator iter = impl->vertex_buffers.begin (), end = impl->vertex_buffers.end (); iter != end; ++iter)
    vertex_buffers_size += iter->SerializationSize ();

  return sizeof (impl->source_id) + sizeof (uint32_t) + impl->name.size () + impl->material_map.SerializationSize () + sizeof (uint32_t) +
         vertex_buffers_size + impl->index_buffer.SerializationSize () + SerializationPrimitivesDataSize ();
}

size_t Mesh::SerializationPrimitivesDataSize () const
{
  return sizeof (uint32_t) + impl->primitives.size () * sizeof (media::geometry::Primitive);
}

size_t Mesh::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::Mesh::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_written = 0;

  if (sizeof (impl->source_id) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing source id");

  memcpy (buffer, &impl->source_id, sizeof (impl->source_id));

  bytes_written += sizeof (impl->source_id);

  uint32_t name_size = impl->name.size ();

  if (sizeof (name_size) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing name size");

  memcpy ((char*)buffer + bytes_written, &name_size, sizeof (name_size));

  bytes_written += sizeof (name_size);

  if (name_size + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing name");

  memcpy ((char*)buffer + bytes_written, impl->name.data (), name_size);

  bytes_written += name_size;

  bytes_written += impl->material_map.Write ((char*)buffer + bytes_written, buffer_size - bytes_written);

  uint32_t vertex_buffers_count = impl->vertex_buffers.size ();

  if (sizeof (vertex_buffers_count) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing vertex buffers count");

  memcpy ((char*)buffer + bytes_written, &vertex_buffers_count, sizeof (vertex_buffers_count));

  bytes_written += sizeof (vertex_buffers_count);

  for (VertexBufferArray::iterator iter = impl->vertex_buffers.begin (), end = impl->vertex_buffers.end (); iter != end; ++iter)
    bytes_written += iter->Write ((char*)buffer + bytes_written, buffer_size - bytes_written);

  bytes_written += impl->index_buffer.Write ((char*)buffer + bytes_written, buffer_size - bytes_written);

  return bytes_written + WritePrimitivesData ((char*)buffer + bytes_written, buffer_size - bytes_written);
}

size_t Mesh::WritePrimitivesData (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::Mesh::WritePrimitivesData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationPrimitivesDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing primitives data");

  size_t bytes_written = 0;

  uint32_t primitives_count = impl->primitives.size ();

  memcpy (buffer, &primitives_count, sizeof (primitives_count));

  bytes_written += sizeof (primitives_count);

  memcpy ((char*)buffer + bytes_written, &impl->primitives.front (), primitives_count * sizeof (media::geometry::Primitive));

  return data_size;
}

size_t Mesh::Read (void* buffer, size_t buffer_size)
{
  size_t bytes_read = 0;

  CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

size_t Mesh::ReadPrimitivesData (void* buffer, size_t buffer_size)
{
  static const char* METHOD_NAME = "media::geometry::Mesh::ReadPrimitivesData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  uint32_t primitives_count;

  if (sizeof (primitives_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading primitives count");

  memcpy (&primitives_count, (char*)buffer + bytes_read, sizeof (primitives_count));

  bytes_read += sizeof (primitives_count);

  impl->primitives.clear ();
  impl->primitives.resize (primitives_count);

  size_t primitives_data_size = primitives_count * sizeof (media::geometry::Primitive);

  if (primitives_data_size + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading primitives data");

  memcpy (&impl->primitives.front (), (char*)buffer + bytes_read, primitives_data_size);

  bytes_read += primitives_data_size;

  impl->primitives_data_update_index++;

  return bytes_read;
}

Mesh Mesh::CreateFromSerializedData (void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::Mesh::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  Mesh new_mesh;

  if (sizeof (new_mesh.impl->source_id) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading source id");

  memcpy (&new_mesh.impl->source_id, buffer, sizeof (new_mesh.impl->source_id));

  bytes_read += sizeof (new_mesh.impl->source_id);

  uint32_t name_size;

  if (sizeof (name_size) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading name size");

  memcpy (&name_size, (char*)buffer + bytes_read, sizeof (name_size));

  bytes_read += sizeof (name_size);

  new_mesh.impl->name.fast_resize (name_size);

  if (name_size + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading name");

  memcpy (&new_mesh.impl->name [0], (char*)buffer + bytes_read, name_size);

  bytes_read += name_size;

  bytes_read += new_mesh.impl->material_map.Read ((char*)buffer + bytes_read, buffer_size - bytes_read);

  uint32_t vertex_buffers_count;

  if (sizeof (vertex_buffers_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading vertex buffers count");

  memcpy (&vertex_buffers_count, (char*)buffer + bytes_read, sizeof (vertex_buffers_count));

  bytes_read += sizeof (vertex_buffers_count);

  new_mesh.impl->vertex_buffers.resize (vertex_buffers_count);

  for (VertexBufferArray::iterator iter = new_mesh.impl->vertex_buffers.begin (), end = new_mesh.impl->vertex_buffers.end (); iter != end; ++iter)
    bytes_read += iter->Read ((char*)buffer + bytes_read, buffer_size - bytes_read);

  bytes_read += new_mesh.impl->index_buffer.Read ((char*)buffer + bytes_read, buffer_size - bytes_read);

  bytes_read += new_mesh.ReadPrimitivesData ((char*)buffer + bytes_read, buffer_size - bytes_read);

  out_bytes_read = bytes_read;

  return new_mesh;
}

/*
   Объект оповещения об удалении
*/

xtl::trackable& Mesh::Trackable () const
{
  return *impl;
}

/*
    Обмен
*/

void Mesh::Swap (Mesh& mesh)
{
  swap (mesh.impl, impl);
}

namespace media
{

namespace geometry
{

xtl::trackable& get_trackable (const Mesh& mesh)
{
  return mesh.Trackable ();
}

void swap (Mesh& mesh1, Mesh& mesh2)
{
  mesh1.Swap (mesh2);
}

/*
    Получение имени типа примитива
*/

const char* get_type_name (PrimitiveType type)
{
  switch (type)
  {
    case PrimitiveType_LineList:      return "line_list";
    case PrimitiveType_LineStrip:     return "line_strip";
    case PrimitiveType_TriangleList:  return "triangle_list";
    case PrimitiveType_TriangleStrip: return "triangle_strip";
    case PrimitiveType_TriangleFan:   return "triangle_fan";
    default:                          throw xtl::make_argument_exception ("media::geometry::get_type_name (PrimitiveType)", "type", type);
  }
  
  return "";
}

/*
    Получение типа примитива по имени
*/

PrimitiveType get_primitive_type (const char* name, PrimitiveType default_type)
{
  if (!name)
    return default_type;

  struct Map
  {
    stl::hash_key<const char*> name_hash;
    PrimitiveType              type;
  };

  static Map map [] = {
    {"line_list",       PrimitiveType_LineList},
    {"line_strip",      PrimitiveType_LineStrip},
    {"triangle_list",   PrimitiveType_TriangleList},
    {"triangle_strip",  PrimitiveType_TriangleStrip},
    {"triangle_fan",    PrimitiveType_TriangleFan},
  };

  static const size_t map_size = sizeof (map) / sizeof (*map);

  const Map* map_iter = map;
  size_t     hash     = strhash (name);

  for (size_t i=0; i<map_size; i++, map_iter++)
    if (map_iter->name_hash.get_hash () == hash)
      return map_iter->type;

  return default_type;
}

/*
    Получение количества точек
*/

unsigned int get_points_count (PrimitiveType type, unsigned int primitives_count)
{
  switch (type)
  {
    case PrimitiveType_LineList:      return primitives_count * 2;
    case PrimitiveType_LineStrip:     return primitives_count + 1;
    case PrimitiveType_TriangleList:  return primitives_count * 3;
    case PrimitiveType_TriangleStrip: return primitives_count + 2;
    case PrimitiveType_TriangleFan:   return primitives_count + 2;
    default:                          throw xtl::make_argument_exception ("media::geometry::get_points_count", "type", type);
  }
  
  return 0;
}

/*
    Определение количества примитивов
*/

unsigned int get_primitives_count (PrimitiveType type, unsigned int points_count)
{
  switch (type)
  {
    case PrimitiveType_LineList:      return points_count / 2;
    case PrimitiveType_LineStrip:     return points_count ? points_count - 1 : 0;
    case PrimitiveType_TriangleList:  return points_count / 3;
    case PrimitiveType_TriangleStrip: return points_count >= 2 ? points_count - 2 : 0;
    case PrimitiveType_TriangleFan:   return points_count >= 2 ? points_count - 2 : 0;
    default:                          throw xtl::make_argument_exception ("media::geometry::get_primitives_count", "type", type);
  }
  
  return 0;
}

}

}
