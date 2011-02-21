#include "shared.h"

//TODO: check no index buffer
//TODO: add material

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации примитива
*/

namespace
{

/*struct MeshPrimitive
{
  //TODO: add material
  render::low_level::PrimitiveType type;          //тип примитива
  VertexBufferPtr                  vertex_buffer; //вершинный буфер
  size_t                           first;         //индекс первой вершины/индекса
  size_t                           count;         //количество примитивов  
};*/

}

struct PrimitiveImpl::Impl
{
  DeviceManagerPtr device_manager;   //менеджер устройства
  BuffersPtr       buffers;          //буферы примитива
  
///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager, const BuffersPtr& in_buffers)
    : device_manager (in_device_manager)
    , buffers (in_buffers)
  {
    static const char* METHOD_NAME = "render::mid_level::PrimitiveImpl::Impl::Impl";
    
    if (!device_manager)
      throw xtl::format_operation_exception (METHOD_NAME, "No DeviceManager binded");

    if (!buffers)
      throw xtl::make_null_argument_exception (METHOD_NAME, "buffers");
  }
};

/*
    Конструктор / деструктор
*/

PrimitiveImpl::PrimitiveImpl (const DeviceManagerPtr& device_manager, const BuffersPtr& buffers)
{
  try
  {
    impl = new Impl (device_manager, buffers);
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
    Буферы примитива
*/

const PrimitiveImpl::BuffersPtr& PrimitiveImpl::Buffers ()
{
  return impl->buffers;
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

void PrimitiveImpl::RemoveMeshes (size_t first_mesh, size_t meshes_count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveMeshes");
}

void PrimitiveImpl::RemoveAllMeshes ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::PrimitiveImpl::RemoveAllMeshes");
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
