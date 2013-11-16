#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

Node* SceneFactory::Create (scene_graph::Node& src_node, SceneManager& scene_manager)
{
  if (scene_graph::Scissor* node = dynamic_cast<scene_graph::Scissor*> (&src_node))
    return new Node (src_node, scene_manager, interchange::NodeType_Scissor); 

  if (scene_graph::Node* node = dynamic_cast<scene_graph::Node*> (&src_node)) //don't need this!!!!
    return new Node (src_node, scene_manager);

  return 0;
}
