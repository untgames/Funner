#include "shared.h"

using namespace media::geometry;
using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Dynamic mesh
class DynamicMesh: public VisualModel
{
  public:
///Constructor
    DynamicMesh (scene_graph::DynamicMesh& mesh, SceneManager& manager)
      : VisualModel (mesh, manager, interchange::NodeType_DynamicMesh)
      , dynamic_mesh_helper (*this, mesh.Mesh ())
      {}

///Source node
    scene_graph::DynamicMesh& SourceNode () const { return static_cast<scene_graph::DynamicMesh&> (Node::SourceNode ()); }

  protected:
///Synchronization implementation
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::DynamicMesh& mesh = SourceNode ();

        dynamic_mesh_helper.UpdateCore (context, mesh.Mesh ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::DynamicMesh::UpdateCore");
        throw;
      }
    }

  private:
    DynamicMeshHelper dynamic_mesh_helper;
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::DynamicMesh& mesh, SceneManager& scene_manager)
{
  return new DynamicMesh (mesh, scene_manager);
}

}}}
