#ifndef SCENE_GRAPH_SERIALIZATION_MENAGER_SHARED_HEADER
#define SCENE_GRAPH_SERIALIZATION_MENAGER_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>
#include <xtl/shared_ptr.h>

#include <common/property_map.h>
#include <common/serializer_manager.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <sg/scene_manager.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер сериализации сцен
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager
  <ISceneFactory* (const char* file_name, const SceneSerializationManager::LogHandler& log_handler),
   void (const char* file_name, Node& node)>
SceneSerializationManagerImpl;

}

#endif
