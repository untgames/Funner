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

void PrimitiveBuffers::Update (const media::geometry::IndexBuffer& buffer)
{
  impl->Update (buffer);
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

void PrimitiveBuffers::ReserveLines (size_t count)
{
  impl->ReserveLines (count);
}

void PrimitiveBuffers::ReserveSprites (size_t count)
{
  impl->ReserveSprites (count);
}

size_t PrimitiveBuffers::LinesCapacity () const
{
  return impl->LinesCapacity ();
}

size_t PrimitiveBuffers::SpritesCapacity () const
{
  return impl->SpritesCapacity ();
}

/*
    Режим использования буферов вспомогательных примитивов
*/

MeshBufferUsage PrimitiveBuffers::LinesBufferUsage () const
{
  return impl->LinesBufferUsage ();  
}

MeshBufferUsage PrimitiveBuffers::SpritesBufferUsage () const
{
  return impl->SpritesBufferUsage ();
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
