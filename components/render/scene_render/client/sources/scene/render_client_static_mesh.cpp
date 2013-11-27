#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

/*
    Описание реализации сущности
*/

struct StaticMesh::Impl
{
  size_t mesh_name_hash; //хэш имени меша

  Impl () : mesh_name_hash () {}
};

/*
    Конструктор / деструктор
*/

StaticMesh::StaticMesh (scene_graph::StaticMesh& entity, SceneManager& manager, interchange::NodeType node_type)
  : VisualModel (entity, manager, node_type)
  , impl (new Impl)
{
}

StaticMesh::~StaticMesh ()
{
}

/*
    Реализация синхронизации
*/

void StaticMesh::UpdateCore (client::Context& context)
{
  try
  {
    VisualModel::UpdateCore (context);

    scene_graph::StaticMesh& mesh = SourceNode ();

      //синхронизация имени меша

    size_t mesh_name_hash = mesh.MeshNameHash ();

    if (mesh_name_hash != impl->mesh_name_hash)
    {
      context.SetStaticMeshName (Id (), mesh.MeshName ());

      impl->mesh_name_hash = mesh_name_hash;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::StaticMesh::UpdateCore");
    throw;
  }
}
