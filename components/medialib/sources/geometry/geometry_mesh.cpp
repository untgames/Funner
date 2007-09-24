#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace stl;
using namespace common;

const size_t DEFAULT_PRIMITIVES_ARRAY_RESERVE = 8; //количество резервируемых примитивов

/*
    Реализация примитива
*/

struct PrimitiveImpl: public Primitive
{
  size_t material_name_offset; //смещение имени материала в строке имён
};

/*
    Описание реализации Mesh
*/

typedef stl::vector<PrimitiveImpl>         PrimitiveArray;
typedef SharedResourceHolder<VertexBuffer> VertexBufferHolder;
typedef SharedResourceHolder<IndexBuffer>  IndexBufferHolder;

struct Mesh::Impl: public SharedResource
{
  string             name;                       //имя меша
  VertexBufferHolder vertex_buffer;              //вершинный буфер
  IndexBufferHolder  index_buffer;               //индексный буфер
  PrimitiveArray     primitives;                 //примитивы
  string             material_names;             //имена материалов
  bool               need_material_names_update; //необходимо обновить имена материалов
  
  Impl (); 
  Impl (const Impl&);
};

/*
    Impl::Impl
*/

Mesh::Impl::Impl ()
{
  primitives.reserve (DEFAULT_PRIMITIVES_ARRAY_RESERVE);
  need_material_names_update = true;
}

Mesh::Impl::Impl (const Impl& impl)
  : name (impl.name),
    vertex_buffer (impl.vertex_buffer, ForceClone),
    index_buffer (impl.index_buffer, ForceClone),
    primitives (impl.primitives),
    material_names (impl.material_names),
    need_material_names_update (true)
  {}

/*
    Конструкторы / деструктор
*/

Mesh::Mesh ()
  : impl (new Impl)
  {}

Mesh::Mesh (const Mesh& mesh, CloneMode mode)
  : impl (clone_resource (mesh.impl, mode, "media::geometry::Mesh::Mesh"))
  {}

Mesh::~Mesh ()
{
  release_resource (impl);
}

/*
    Присваивание
*/

void Mesh::Assign (const Mesh& mesh, CloneMode mode)
{
  Mesh (mesh, mode).Swap (*this);
}

Mesh& Mesh::operator = (const Mesh& mesh)
{
  Assign (mesh);

  return *this;
}

/*
    Идентификатор меша
*/

size_t Mesh::Id () const
{
  return reinterpret_cast<size_t> (impl);
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
    RaiseNullArgument ("media::geometry::Mesh::Rename", "name");
    
  impl->name = name;
}

/*
    Буферы
*/

const media::geometry::VertexBuffer& Mesh::VertexBuffer () const
{
  return impl->vertex_buffer.Resource ();
}

media::geometry::VertexBuffer& Mesh::VertexBuffer ()
{
  return impl->vertex_buffer.Resource ();
}

const media::geometry::IndexBuffer& Mesh::IndexBuffer () const
{
  return impl->index_buffer.Resource ();
}

media::geometry::IndexBuffer& Mesh::IndexBuffer ()
{
  return impl->index_buffer.Resource ();
}

/*
    Статистика: количество вершин/индексов/весов
*/

size_t Mesh::VerticesCount () const
{
  return impl->vertex_buffer.Resource ().VerticesCount ();
}

size_t Mesh::IndicesCount () const
{
  return impl->index_buffer.Resource ().Size ();
}

size_t Mesh::WeightsCount () const
{
  return impl->vertex_buffer.Resource ().Weights ().Size ();
}


/*
    Присоединение/отсоединение буферов
*/

void Mesh::Attach (media::geometry::VertexBuffer& vb, CloneMode mode)
{
  impl->vertex_buffer.Assign (vb, mode);
}

void Mesh::Attach (media::geometry::IndexBuffer& ib, CloneMode mode)
{
  impl->index_buffer.Assign (ib, mode);
}
    
void Mesh::DetachVertexBuffer ()
{
  impl->vertex_buffer.Assign (media::geometry::VertexBuffer (), CloneMode_Instance);
}

void Mesh::DetachIndexBuffer ()
{
  impl->index_buffer.Assign (media::geometry::IndexBuffer (), CloneMode_Instance);
}

void Mesh::DetachAllBuffers ()
{
  DetachVertexBuffer ();
  DetachIndexBuffer ();
}

/*
    Количество примитивов / доступ к примитивам
*/

size_t Mesh::PrimitivesCount () const
{
  return impl->primitives.size ();
}

const Primitive& Mesh::Primitive (size_t index) const
{
  if (index >= impl->primitives.size ())
    RaiseOutOfRange ("media::geometry::Mesh::Primitive", "index", index, impl->primitives.size ());
    
  if (impl->need_material_names_update)
  {
    const char* name_base = impl->material_names.c_str ();
    
    for (PrimitiveArray::iterator i=impl->primitives.begin (), end=impl->primitives.end (); i!=end; ++i)
      i->material = name_base + i->material_name_offset;

    impl->need_material_names_update = false;
  }

  return impl->primitives [index];
}

/*
    Добавление/удаление примитивов примитивов
*/

size_t Mesh::AddPrimitive (PrimitiveType type, size_t first, size_t count, const char* material)
{
  if (type < 0 || type >= PrimitiveType_Num)
    RaiseInvalidArgument ("media::geometry::Mesh::AddPrimitive", "type", type);
    
  if (!material)
    material = "";
    
  PrimitiveImpl primitive;
  
  primitive.type                 = type;
  primitive.first                = first;
  primitive.count                = count;
  primitive.material             = 0;
  primitive.material_name_offset = impl->material_names.size ();

  impl->material_names.append (material, strlen (material) + 1);

  try
  {
    impl->primitives.push_back (primitive);

    impl->need_material_names_update = true;

    return impl->primitives.size () - 1;
  }
  catch (...)
  {
    impl->material_names.erase (primitive.material_name_offset);
    throw;
  }
}

void Mesh::RemovePrimitive (size_t primitive_index)
{
  if (primitive_index >= impl->primitives.size ())
    return;

  impl->primitives.erase (impl->primitives.begin () + primitive_index);

  if (impl->primitives.empty ())
    impl->material_names.clear ();
}

void Mesh::RemoveAllPrimitives ()
{
  impl->primitives.clear ();
  impl->material_names.clear ();
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
    Обмен
*/

void Mesh::Swap (Mesh& mesh)
{
  stl::swap (mesh.impl, impl);
}

namespace media
{

namespace geometry
{

/*
    Обмен
*/
    
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
    default:                          RaiseInvalidArgument ("media::geometry::get_type_name (PrimitiveType)", "type", type);
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

size_t get_points_count (PrimitiveType type, size_t primitives_count)
{
  switch (type)
  {
    case PrimitiveType_LineList:      return primitives_count * 2;
    case PrimitiveType_LineStrip:     return primitives_count + 1;
    case PrimitiveType_TriangleList:  return primitives_count * 3;
    case PrimitiveType_TriangleStrip: return primitives_count + 2;
    case PrimitiveType_TriangleFan:   return primitives_count + 2;
    default:                          RaiseInvalidArgument ("media::geometry::get_points_count", "type", type);
  }
  
  return 0;
}

}

}
