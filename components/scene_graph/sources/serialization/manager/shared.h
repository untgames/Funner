#ifndef SCENE_GRAPH_SERIALIZATION_MANAGER_SHARED_HEADER
#define SCENE_GRAPH_SERIALIZATION_MANAGER_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/ref.h>
#include <xtl/signal.h>
#include <xtl/shared_ptr.h>
#include <xtl/stat_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parse_tree.h>
#include <common/property_map.h>
#include <common/serializer_manager.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <media/rms/binding.h>
#include <media/rms/manager.h>

#include <sg/scene_manager.h>
#include <sg/scene_parser.h>
#include <sg/controllers/resource_controller.h>

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
