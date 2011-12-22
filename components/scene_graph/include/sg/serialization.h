#ifndef SCENE_GRAPH_SCENE_SERIALIZER_HEADER
#define SCENE_GRAPH_SCENE_SERIALIZER_HEADER

#include <xtl/functional_fwd>

#include <common/serializer_manager.h>

#include <sg/node.h>

namespace scene_graph
{

//forward declaration
class SceneManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Порождающий функтор узла
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::function<Node::Pointer ()> NodeCreator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер сериализации сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<
  NodeCreator (const char* file_name, SceneManager& manager),
  void (const char* file_name, const Node& node, SceneManager& manager)>
SceneSerializerManager;

}

#endif
