#ifndef SCENE_GRAPH_SCENE_SERIALIZER_HEADER
#define SCENE_GRAPH_SCENE_SERIALIZER_HEADER

#include <xtl/functional_fwd>

#include <common/serializer_manager.h>

#include <sg/node.h>
#include <sg/scene_manager.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер сериализации сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<
  NodeCreator (const char* file_name, SceneManager& manager),
  void (const char* file_name, const Node& node, SceneManager& manager)>
SceneSerializerManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер парсинга сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<
  NodeCreator (const common::ParseNode& parse_root, SceneManager& manager),
  void (const common::ParseNode& parse_root, const Node& node, SceneManager& manager)>
SceneParserManager;

}

#endif
