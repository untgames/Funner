#include "shared.h"

using namespace render;

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

size_t Primitive::LinesCount () const
{
  return impl->LinesCount ();
}

size_t Primitive::AddLines (size_t lines_count, const Line* lines, const Material& material)
{
  return impl->AddLines (lines_count, lines, Wrappers::Unwrap<MaterialImpl> (material));
}

void Primitive::UpdateLines (size_t first_lines, size_t lines_count, const Line* lines)
{
  impl->UpdateLines (first_lines, lines_count, lines);
}

void Primitive::SetLinesMaterial (size_t first_lines, size_t lines_count, const Material& material)
{
  impl->SetLinesMaterial (first_lines, lines_count, Wrappers::Unwrap<MaterialImpl> (material));
}

void Primitive::RemoveLines (size_t first_lines, size_t lines_count)
{
  impl->RemoveLines (first_lines, lines_count);
}

void Primitive::RemoveAllLines ()
{
  impl->RemoveAllLines ();
}

void Primitive::ReserveLines (size_t lines_count)
{
  impl->ReserveLines (lines_count);
}

size_t Primitive::SpritesCount () const
{
  return impl->SpritesCount ();
}

size_t Primitive::AddSprites (size_t sprites_count, const Sprite* sprites, const Material& material)
{
  return impl->AddSprites (sprites_count, sprites, Wrappers::Unwrap<MaterialImpl> (material));
}

void Primitive::UpdateSprites (size_t first_sprite, size_t sprites_count, const Sprite* sprites)
{
  impl->UpdateSprites (first_sprite, sprites_count, sprites);
}

void Primitive::SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const Material& material)
{
  impl->SetSpritesMaterial (first_sprite, sprites_count, Wrappers::Unwrap<MaterialImpl> (material));
}

void Primitive::RemoveSprites (size_t first_sprite, size_t sprites_count)
{
  impl->RemoveSprites (first_sprite, sprites_count);
}

void Primitive::RemoveAllSprites ()
{
  impl->RemoveAllSprites ();
}

void Primitive::ReserveSprites (size_t sprites_count)
{
  impl->ReserveSprites (sprites_count);
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

void swap (Primitive& primitive1, Primitive& primitive2)
{
  primitive1.Swap (primitive2);
}

}
