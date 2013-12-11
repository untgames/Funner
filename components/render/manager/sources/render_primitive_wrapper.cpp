#include "shared.h"

using namespace render::manager;

Primitive::Primitive (PrimitiveImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Primitive::Primitive (const Primitive& primitive)
  : impl (primitive.impl)
{
  addref (impl);
}

Primitive::~Primitive ()
{
  release (impl);
}

Primitive& Primitive::operator = (const Primitive& primitive)
{
  Primitive (primitive).Swap (*this);
  return *this;
}

const char* Primitive::Name () const
{
  return impl->Name ();
}

void Primitive::SetName (const char* name)
{
  impl->SetName (name);
}

PrimitiveBuffers Primitive::Buffers () const
{
  return Wrappers::Wrap<PrimitiveBuffers> (impl->Buffers ());
}

size_t Primitive::MeshesCount () const
{
  return impl->MeshesCount ();
}

size_t Primitive::AddMesh (const media::geometry::Mesh& mesh, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return impl->AddMesh (mesh, vb_usage, ib_usage);
}

void Primitive::RemoveMeshes (size_t first_mesh, size_t meshes_count)
{
  impl->RemoveMeshes (first_mesh, meshes_count);
}

void Primitive::RemoveAllMeshes ()
{
  impl->RemoveAllMeshes ();
}

size_t Primitive::SpriteListsCount () const
{
  return impl->SpriteListsCount ();
}

size_t Primitive::AddStandaloneSpriteList (const SpriteList& list, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return impl->AddStandaloneSpriteList (Wrappers::Unwrap<SpriteListImpl> (list), vb_usage, ib_usage);
}

size_t Primitive::AddBatchingSpriteList (const SpriteList& list, SpriteMode mode)
{
  return impl->AddBatchingSpriteList (Wrappers::Unwrap<SpriteListImpl> (list), mode);
}

void Primitive::RemoveSpriteList (size_t index)
{
  impl->RemoveSpriteList (index);
}

void Primitive::RemoveAllSpriteLists ()
{
  impl->RemoveAllSpriteLists ();
}

size_t Primitive::LineListsCount () const
{
  return impl->LineListsCount ();
}

size_t Primitive::AddStandaloneLineList (const LineList& list, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return impl->AddStandaloneLineList (Wrappers::Unwrap<LineListImpl> (list), vb_usage, ib_usage);
}

size_t Primitive::AddBatchingLineList (const LineList& list)
{
  return impl->AddBatchingLineList (Wrappers::Unwrap<LineListImpl> (list));
}

void Primitive::RemoveLineList (size_t index)
{
  impl->RemoveLineList (index);
}

void Primitive::RemoveAllLineLists ()
{
  impl->RemoveAllLineLists ();
}

/*
    Управление кэшированием
*/

void Primitive::UpdateCache ()
{
  impl->UpdateCache ();
}

void Primitive::ResetCache ()
{
  impl->ResetCache ();
}

void Primitive::Swap (Primitive& primitive)
{
  stl::swap (impl, primitive.impl);
}

namespace render
{

namespace manager
{

void swap (Primitive& primitive1, Primitive& primitive2)
{
  primitive1.Swap (primitive2);
}

}

}
