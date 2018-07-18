#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace render {
namespace scene {
namespace client {

typedef stl::vector<object_id_t> ObjectIdArray;
typedef stl::vector<math::mat4f> MatrixArray;

///Скин меш
struct SkinMesh::Impl
{
  bool                 need_update_nodes;             //флаг необходимости обновления узлов
  bool                 need_update_matrices;          //флаг необходимости обновления матриц
  xtl::auto_connection on_update_nodes_connection;    //оповещение об обновлении узлов соединений
  xtl::auto_connection on_update_matrices_connection; //оповещение об обновлении матриц соединений
  size_t               current_joints_count;          //текущее количество соединений
  ObjectIdArray        joint_node_ids;                //идентификаторы узлов соединений
  MatrixArray          joint_matrices;                //матрицы соединений

///Конструктор
  Impl ()
    : need_update_nodes (true)
    , need_update_matrices (true)
    , current_joints_count ()
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

      //обновление параметров скин меша

    scene_graph::SkinMesh& mesh         = SourceNode ();
    size_t                 joints_count = mesh.JointsCount ();

    if (impl->current_joints_count != joints_count)
    {
      context.SetSkinMeshJointsCount (Id (), joints_count);

      impl->need_update_nodes    = true;
      impl->need_update_matrices = true;
      impl->current_joints_count = joints_count;
    }

      //обновление узлов соединений

    if (impl->need_update_nodes)
    {
      impl->joint_node_ids.resize (joints_count);

      SceneManager& scene_manager = Scenes ();
      bool          need_update   = false;

      for (size_t i=0; i<joints_count; i++)
      {
        NodePtr                    node;
        object_id_t                node_id  = 0;
        scene_graph::Node::Pointer src_node = mesh.Joint (i);

        if (src_node)
          node = scene_manager.FindNode (*src_node);

        if (node)
          node_id = node->Id ();

        object_id_t& dst_node_id = impl->joint_node_ids [i];

        if (node_id != dst_node_id)
        {
          need_update = true;
          dst_node_id = node_id;
        }
      }

      if (need_update)
      {
        if (joints_count)
        {
          context.SetSkinMeshJointNodes (Id (), interchange::RawArray<object_id_t> (&impl->joint_node_ids [0], uint32 (joints_count)));
        }
        else
        {
          context.SetSkinMeshJointNodes (Id (), interchange::RawArray<object_id_t> (0, 0));
        }
      }

      impl->need_update_nodes = false;
    }

      //обновление матриц соединений

    if (impl->need_update_matrices)
    {
      impl->joint_matrices.resize (joints_count);

      for (MatrixArray::iterator it=impl->joint_matrices.begin (), end=impl->joint_matrices.end (); it!=end; ++it)
      {
        size_t             index = it - impl->joint_matrices.begin ();
        const math::mat4f& tm    = mesh.JointInvMatrix (index);

        *it = tm;
      }

      if (joints_count)
      {
        context.SetSkinMeshJointMatrices (Id (), interchange::RawArray<math::mat4f> (&impl->joint_matrices [0], uint32 (joints_count)));
      }
      else
      {
        context.SetSkinMeshJointMatrices (Id (), interchange::RawArray<math::mat4f> (0, 0));
      }
      
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
