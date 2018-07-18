#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace render {
namespace scene {
namespace client {

///Скин меш
struct SkinMesh::Impl
{
  bool                 need_update_nodes;             //флаг необходимости обновления узлов
  bool                 need_update_matrices;          //флаг необходимости обновления матриц
  xtl::auto_connection on_update_nodes_connection;    //оповещение об обновлении узлов соединений
  xtl::auto_connection on_update_matrices_connection; //оповещение об обновлении матриц соединений

///Конструктор
  Impl ()
    : need_update_nodes (true)
    , need_update_matrices (true)
  {
  }

///Оповещения об обновлении соединений
  void OnUpdateNodes ()
  {
    need_update_nodes = true;
  }

  void OnUpdateMatrices ()
  {
    need_update_matrices = true;
  }
};

///Конструктор
SkinMesh::SkinMesh (scene_graph::SkinMesh& mesh, SceneManager& manager, interchange::NodeType node_type)
  : StaticMesh (mesh, manager, node_type)
  , impl (new Impl)
{
  impl->on_update_nodes_connection    = mesh.RegisterEventHandler (scene_graph::SkinMeshEvent_AfterJointNodesUpdate, xtl::bind (&Impl::OnUpdateNodes, &*impl));
  impl->on_update_matrices_connection = mesh.RegisterEventHandler (scene_graph::SkinMeshEvent_AfterJointMatricesUpdate, xtl::bind (&Impl::OnUpdateMatrices, &*impl));
}

SkinMesh::~SkinMesh ()
{
}

///Реализация синхронизации
void SkinMesh::UpdateCore (client::Context& context)
{
  try
  {
    StaticMesh::UpdateCore (context);

    scene_graph::SkinMesh& mesh = SourceNode ();

    if (impl->need_update_nodes)
    {
      //TODO
      impl->need_update_nodes = false;
    }

    if (impl->need_update_matrices)
    {
      //TODO
      impl->need_update_matrices = false;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::SkinMesh::UpdateCore");
    throw;
  }
}


Node* create_node (scene_graph::SkinMesh& mesh, SceneManager& scene_manager)
{
  return new SkinMesh (mesh, scene_manager);
}

}}}
