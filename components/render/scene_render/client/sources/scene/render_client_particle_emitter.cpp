#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

namespace
{

/*
    Particle emitter
*/

class ParticleEmitter: public VisualModel
{
  public:
///Constructor
    ParticleEmitter (scene_graph::ParticleEmitter& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_SpriteList)
    {
    }

///Source node
    scene_graph::ParticleEmitter& SourceNode () const { return static_cast<scene_graph::ParticleEmitter&> (Node::SourceNode ()); }

  protected:
///Synchronization implementation
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::ParticleEmitter& model = SourceNode ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::ParticleEmitter::UpdateCore");
        throw;
      }
    }
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::ParticleEmitter& model, SceneManager& scene_manager)
{
  return new ParticleEmitter (model, scene_manager);
}

}}}
