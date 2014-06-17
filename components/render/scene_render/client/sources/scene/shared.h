#ifndef RENDER_SCENE_CLIENT_IMPL_SCENE_SHARED_HEADER
#define RENDER_SCENE_CLIENT_IMPL_SCENE_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>

#include <common/property_map.h>
#include <common/strlib.h>

#include <sg/light.h>
#include <sg/line.h>
#include <sg/mesh.h>
#include <sg/page_curl.h>
#include <sg/scissor.h>
#include <sg/sprite.h>

#include <shared/client.h>
#include <shared/scene_manager.h>

#include "visual_model.h"

namespace render
{

namespace scene
{

namespace client
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Фабрика объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneFactory
{
  public:
    static Node* Create (scene_graph::Node& src_node, SceneManager& scene_manager);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание различных типов объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
Node* create_node (scene_graph::StaticMesh&, SceneManager& scene_manager);
Node* create_node (scene_graph::PointLight&, SceneManager& scene_manager);
Node* create_node (scene_graph::DirectLight&, SceneManager& scene_manager);
Node* create_node (scene_graph::SpotLight&, SceneManager& scene_manager);
Node* create_node (scene_graph::PageCurl&, SceneManager& scene_manager);
Node* create_node (scene_graph::SpriteModel&, SceneManager& scene_manager);

}

}

}

#endif
