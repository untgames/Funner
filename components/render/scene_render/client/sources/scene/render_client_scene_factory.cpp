#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

Node* SceneFactory::Create (scene_graph::Node& src_node, SceneManager& scene_manager)
{
  if (scene_graph::Scissor* node = dynamic_cast<scene_graph::Scissor*> (&src_node))
    return new Node (*node, scene_manager, interchange::NodeType_Scissor); 

  if (scene_graph::StaticMesh* node = dynamic_cast<scene_graph::StaticMesh*> (&src_node))
    return new StaticMesh (*node, scene_manager, interchange::NodeType_StaticMesh);

  return 0;
}
