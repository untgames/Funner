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

SpriteList Primitive::AddStandaloneSpriteList (SpriteMode mode, const math::vec3f& up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return Wrappers::Wrap<SpriteList> (impl->AddStandaloneSpriteList (mode, up, vb_usage, ib_usage));
}

SpriteList Primitive::AddBatchingSpriteList (SpriteMode mode, const math::vec3f& up)
{
  return Wrappers::Wrap<SpriteList> (impl->AddBatchingSpriteList (mode, up));
}

void Primitive::RemoveSpriteList (SpriteList& list)
{
  impl->RemoveSpriteList (Wrappers::Unwrap<SpriteListImpl> (list));
}

void Primitive::RemoveAllSpriteLists ()
{
  impl->RemoveAllSpriteLists ();
}

size_t Primitive::LineListsCount () const
{
  return impl->LineListsCount ();
}

LineList Primitive::AddStandaloneLineList (MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return Wrappers::Wrap<LineList> (impl->AddStandaloneLineList (vb_usage, ib_usage));
}

LineList Primitive::AddBatchingLineList ()
{
  return Wrappers::Wrap<LineList> (impl->AddBatchingLineList ());
}

void Primitive::RemoveLineList (LineList& list)
{
  impl->RemoveLineList (Wrappers::Unwrap<LineListImpl> (list));
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
