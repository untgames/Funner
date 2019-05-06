#include "shared.h"

using namespace render::manager;

/*
    Конструкторы / деструктор / присваивание
*/

PrimitiveBuffers::PrimitiveBuffers (PrimitiveBuffersImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

PrimitiveBuffers::PrimitiveBuffers (const PrimitiveBuffers& p)
  : impl (p.impl)
{
  addref (impl);
}

PrimitiveBuffers::~PrimitiveBuffers ()
{
  release (impl);
}

PrimitiveBuffers& PrimitiveBuffers::operator = (const PrimitiveBuffers& p)
{
  PrimitiveBuffers (p).Swap (*this);
  
  return *this;
}

/*
    Добавление буферов
*/

void PrimitiveBuffers::Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage)
{
  impl->Add (buffer, usage);
}

void PrimitiveBuffers::Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage)
{
  impl->Add (buffer, usage);
}

void PrimitiveBuffers::Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage)
{
  impl->Add (buffer, usage);
}

/*
    Обновление буферов
*/

void PrimitiveBuffers::Update (const media::geometry::VertexStream& buffer)
{
  impl->Update (buffer);
}

void PrimitiveBuffers::UpdateVertexStream (object_id_t id, const void* buffer, size_t buffer_size)
{
  impl->UpdateVertexStream (id, buffer, buffer_size);
}

void PrimitiveBuffers::Update (const media::geometry::IndexBuffer& buffer)
{
  impl->Update (buffer);
}

void PrimitiveBuffers::UpdateIndexBuffer (object_id_t id, const void* buffer, size_t buffer_size)
{
  impl->UpdateIndexBuffer (id, buffer, buffer_size);
}

/*
    Удаление буферов
*/

void PrimitiveBuffers::Remove (const media::geometry::VertexStream& buffer)
{
  impl->Remove (buffer);
}

void PrimitiveBuffers::Remove (const media::geometry::VertexBuffer& buffer)
{
  impl->Remove (buffer);
}

void PrimitiveBuffers::Remove (const media::geometry::IndexBuffer& buffer)
{
  impl->Remove (buffer);
}

void PrimitiveBuffers::RemoveAll ()
{
  impl->RemoveAll ();
}

/*
    Резервирование вспомогательных примитивов
*/

void PrimitiveBuffers::ReserveDynamicBuffers (unsigned int vertices_count, unsigned int indices_count)
{
  if (!vertices_count && !indices_count)
    return;

  impl->BatchingManager ().ReserveDynamicBuffers (vertices_count, indices_count);
}

unsigned int PrimitiveBuffers::DynamicVerticesCount () const
{
  return impl->HasBatchingManager () ? impl->BatchingManager ().DynamicVerticesCount () : 0;
}

unsigned int PrimitiveBuffers::DynamicIndicesCount () const
{
  return impl->HasBatchingManager () ? impl->BatchingManager ().DynamicIndicesCount () : 0;
}

/*
    Обмен
*/

void PrimitiveBuffers::Swap (PrimitiveBuffers& buffers)
{
  stl::swap (impl, buffers.impl);
}

namespace render
{

namespace manager
{

void swap (PrimitiveBuffers& buffers1, PrimitiveBuffers& buffers2)
{
  buffers1.Swap (buffers2);
}

}

}
