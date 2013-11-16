#include "shared.h"

using namespace render::scene::client;

Node* SceneFactory::Create (scene_graph::Node& src_node, SceneManager& scene_manager)
{
  return new Node (src_node, scene_manager);

  return 0;
}
