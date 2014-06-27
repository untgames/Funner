#include "shared.h"

using namespace render::scene;
using namespace render::scene::client;

Node* SceneFactory::Create (scene_graph::Node& src_node, SceneManager& scene_manager)
{
  if (scene_graph::Scissor* node = dynamic_cast<scene_graph::Scissor*> (&src_node))
    return new Node (*node, scene_manager, interchange::NodeType_Scissor); 

  if (scene_graph::StaticMesh* node = dynamic_cast<scene_graph::StaticMesh*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::SpriteModel* node = dynamic_cast<scene_graph::SpriteModel*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::LineModel* node = dynamic_cast<scene_graph::LineModel*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::TextModel* node = dynamic_cast<scene_graph::TextModel*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::SpotLight* node = dynamic_cast<scene_graph::SpotLight*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::PointLight* node = dynamic_cast<scene_graph::PointLight*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::DirectLight* node = dynamic_cast<scene_graph::DirectLight*> (&src_node))
    return create_node (*node, scene_manager);

  if (scene_graph::PageCurl* node = dynamic_cast<scene_graph::PageCurl*> (&src_node))
    return create_node (*node, scene_manager);

  return 0;
}
