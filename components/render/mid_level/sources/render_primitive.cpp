#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации примитива
*/

struct PrimitiveImpl::Impl
{
  DeviceManagerPtr   device_manager;   //менеджер устройства
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
  {
    if (!device_manager)
      throw xtl::format_operation_exception ("render::mid_level::PrimitiveImpl::Impl::Impl", "No DeviceManager binded");
      
  }
};

/*
    Конструктор / деструктор
*/

PrimitiveImpl::PrimitiveImpl (const DeviceManagerPtr& device_manager)
{
  try
  {
    impl = new Impl (device_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveImpl::PrimitiveImpl");
    throw;
  }
}

PrimitiveImpl::~PrimitiveImpl ()
{
}

/*
    Работа с мешами
*/

size_t PrimitiveImpl::MeshesCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::MeshesCount");
}

size_t PrimitiveImpl::AddMesh (const media::geometry::Mesh&, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::AddMesh");
}

void PrimitiveImpl::UpdateMesh (size_t mesh_index, const media::geometry::Mesh& mesh)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::UpdateMesh");
}

void PrimitiveImpl::RemoveMeshes (size_t first_mesh, size_t meshes_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveMeshes");
}

void PrimitiveImpl::RemoveAllMeshes ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveAllMeshes");
}

/*
    Работа с динамическими буферами меша
*/

void PrimitiveImpl::AddMeshBuffer (const media::geometry::VertexStream& buffer, MeshBufferUsage usage)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::AddMeshBuffer(const media::geometry::VertexStream&,MeshBufferUsage)");
}

void PrimitiveImpl::AddMeshBuffer (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::AddMeshBuffer(const media::geometry::VertexBuffer&,MeshBufferUsage)");
}

void PrimitiveImpl::AddMeshBuffer (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::AddMeshBuffer(const media::geometry::IndexBuffer&,MeshBufferUsage)");
}

void PrimitiveImpl::UpdateMeshBuffer (const media::geometry::VertexStream& buffer)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::UpdateMeshBuffer(const media::geometry::VertexStream&)");
}

void PrimitiveImpl::UpdateMeshBuffer (const media::geometry::VertexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::UpdateMeshBuffer(const media::geometry::VertexBuffer&)");
}

void PrimitiveImpl::UpdateMeshBuffer (const media::geometry::IndexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::UpdateMeshBuffer(const media::geometry::IndexBuffer&)");
}

void PrimitiveImpl::RemoveMeshBuffer (const media::geometry::VertexStream& buffer)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveMeshBuffer(const media::geometry::VertexStream&)");
}

void PrimitiveImpl::RemoveMeshBuffer (const media::geometry::VertexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveMeshBuffer(const media::geometry::VertexBuffer&)");
}

void PrimitiveImpl::RemoveMeshBuffer (const media::geometry::IndexBuffer& buffer)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveMeshBuffer(const media::geometry::IndexBuffer&)");
}

void PrimitiveImpl::RemoveAllMeshBuffers ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveAllMeshBuffers");
}

/*
    Работа с линиями
*/

size_t PrimitiveImpl::LinesCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::LinesCount");
}

size_t PrimitiveImpl::AddLines (size_t lines_count, const Line* lines, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::AddLines");
}

void PrimitiveImpl::UpdateLines (size_t first_lines, size_t lines_count, const Line* Lines)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::UpdateLines");
}

void PrimitiveImpl::SetLinesMaterial (size_t first_lines, size_t lines_count, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::SetLinesMaterial");
}

void PrimitiveImpl::RemoveLines (size_t first_lines, size_t lines_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::ReserveLines");
}

void PrimitiveImpl::RemoveAllLines ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::ReserveAllLines");
}

void PrimitiveImpl::ReserveLines (size_t lines_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::ReserveLines");
}

/*
    Работа со спрайтами
*/

size_t PrimitiveImpl::SpritesCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::SpritesCount");
}

size_t PrimitiveImpl::AddSprites (size_t sprites_count, const Sprite* sprites, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::AddSprites");
}

void PrimitiveImpl::UpdateSprites (size_t first_sprite, size_t sprites_count, const Sprite* sprites)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::UpdateSprites");
}

void PrimitiveImpl::SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const MaterialPtr& material)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::SetSpritesMaterial");
}

void PrimitiveImpl::RemoveSprites (size_t first_sprite, size_t sprites_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveSprites");
}

void PrimitiveImpl::RemoveAllSprites ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveAllSprites");
}

void PrimitiveImpl::ReserveSprites (size_t sprites_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::ReserveSprites");
}
