#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Dynamic mesh implementation
*/

struct DynamicMesh::Impl
{
  manager::Entity& entity;          //entity
  RenderManager    render_manager;  //render manager

///Constructor
  Impl (RenderManager& in_render_manager, manager::Entity& in_entity)
    : entity (in_entity)
    , render_manager (in_render_manager)
    {}
};

/*
    Constructor / destructor
*/

DynamicMesh::DynamicMesh (RenderManager& render_manager)
  : VisualModel (render_manager)
  , impl (new Impl (render_manager, Entity ()))
{
}

DynamicMesh::~DynamicMesh ()
{
}

/*
   Updating mesh data
*/

void DynamicMesh::UpdateMesh (const media::geometry::Mesh& mesh)
{
  if (!impl->entity.HasPrimitive ())
  {
    manager::Primitive primitive = impl->render_manager.Manager ().CreatePrimitive ();

    impl->entity.SetPrimitive (primitive);
  }

  manager::Primitive primitive = impl->entity.Primitive ();

  primitive.RemoveAllMeshes (render::manager::PrimitiveBufferType_IndexBuffer | render::manager::PrimitiveBufferType_VertexStream);

  primitive.Buffers ();

  primitive.AddMesh (mesh, render::manager::MeshBufferUsage_Stream, render::manager::MeshBufferUsage_Dynamic, render::manager::PrimitiveBufferType_IndexBuffer | render::manager::PrimitiveBufferType_VertexStream);
}

void DynamicMesh::UpdateIndexBufferData (media::geometry::object_id_t source_id, const void* buffer, size_t buffer_size)
{
  manager::Primitive primitive = impl->entity.Primitive ();

  primitive.Buffers ().UpdateIndexBuffer (source_id, buffer, buffer_size);
}

void DynamicMesh::UpdateVertexStreamData (media::geometry::object_id_t source_id, const void* buffer, size_t buffer_size)
{
  manager::Primitive primitive = impl->entity.Primitive ();

  primitive.Buffers ().UpdateVertexStream (source_id, buffer, buffer_size);
}
