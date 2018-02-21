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

  primitive.RemoveAllMeshes ();

  //TODO do this without wrapping into media::Mesh (add method for primitives adding to manager::Primitive)
  primitive.AddMesh (mesh);
}
