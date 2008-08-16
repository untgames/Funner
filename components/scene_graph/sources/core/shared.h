#ifndef SCENE_GRAPH_CORE_SHARED_HEADER
#define SCENE_GRAPH_CORE_SHARED_HEADER

#include <stl/string>
#include <stl/algorithm>

#include <xtl/visitor.h>
#include <xtl/signal.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/common_exceptions.h>
#include <xtl/uninitialized_storage.h>

#include <common/utf_converter.h>

#include <bv/axis_aligned_box.h>

#include <sg/camera.h>
#include <sg/light.h>
#include <sg/listener.h>
#include <sg/scene.h>
#include <sg/sound_emitter.h>
#include <sg/sprite.h>
#include <sg/text_line.h>
#include <sg/visual_model.h>

#include "scene_object.h"

#endif
