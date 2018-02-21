#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Статический меш
class DynamicMesh: public VisualModel
{
  public:
///Конструктор
    DynamicMesh (scene_graph::DynamicMesh& mesh, SceneManager& manager)
      : VisualModel (mesh, manager, interchange::NodeType_DynamicMesh)
      {}

///Исходный узел
    scene_graph::DynamicMesh& SourceNode () const { return static_cast<scene_graph::DynamicMesh&> (Node::SourceNode ()); }

  protected:
///Реализация синхронизации
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::DynamicMesh& mesh = SourceNode ();

          //TODO check mesh was not changed, update only parts which were changed

        context.UpdateDynamicMesh (Id (), mesh.Mesh ().Clone ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::DynamicMesh::UpdateCore");
        throw;
      }
    }
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
