#ifndef SCENE_GRAPH_CONTROLLERS_SHARED_HEADER
#define SCENE_GRAPH_CONTROLLERS_SHARED_HEADER

#include <stl/hash_map>
#include <stl/list>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>

#include <common/property_map.h>
#include <common/property_binding_map.h>

#include <math/utility.h>

#include <media/animation/animation_blender.h>
#include <media/animation/animation_library.h>

#include <sg/node.h>
#include <sg/controllers/animation.h>

#endif
