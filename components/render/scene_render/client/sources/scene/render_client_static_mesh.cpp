#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace render {
namespace scene {
namespace client {

///Статический меш
struct StaticMesh::Impl
{
  size_t mesh_name_hash; //хэш имени меша

  Impl () : mesh_name_hash () {}
};

///Конструктор
StaticMesh::StaticMesh (scene_graph::StaticMesh& mesh, SceneManager& manager, interchange::NodeType node_type)
  : VisualModel (mesh, manager, node_type)
  , impl (new Impl)
{
}

StaticMesh::~StaticMesh ()
{
}

///Реализация синхронизации
void StaticMesh::UpdateCore (client::Context& context)
{
  try
  {
    VisualModel::UpdateCore (context);

    scene_graph::StaticMesh& mesh = SourceNode ();

      //синхронизация имени меша

    size_t cur_mesh_name_hash = mesh.MeshNameHash ();

    if (cur_mesh_name_hash != impl->mesh_name_hash)
    {
      context.SetStaticMeshName (Id (), mesh.MeshName ());

      impl->mesh_name_hash = cur_mesh_name_hash;
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::StaticMesh::UpdateCore");
    throw;
  }
}


Node* create_node (scene_graph::StaticMesh& mesh, SceneManager& scene_manager)
{
  return new StaticMesh (mesh, scene_manager);
}

}}}
