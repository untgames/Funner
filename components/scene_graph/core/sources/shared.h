#ifndef SCENE_GRAPH_CORE_SHARED_HEADER
#define SCENE_GRAPH_CORE_SHARED_HEADER

#include <stl/algorithm>
#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/any_reference.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/visitor.h>

#include <common/component.h>
#include <common/singleton.h>
#include <common/utf_converter.h>

#include <bv/axis_aligned_box.h>

#include <sg/camera.h>
#include <sg/controller.h>
#include <sg/helper.h>
#include <sg/light.h>
#include <sg/listener.h>
#include <sg/node_array.h>
#include <sg/node_properties.h>
#include <sg/physics.h>
#include <sg/scene.h>
#include <sg/sound_emitter.h>
#include <sg/sprite.h>
#include <sg/text_line.h>
#include <sg/visual_model.h>

#include "scene_object.h"

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание контроллера
///////////////////////////////////////////////////////////////////////////////////////////////////
IController* create_controller (Node& node, const char* name, const xtl::any_reference& param);

}

#endif
