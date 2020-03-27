#ifndef SCENE_GRAPH_CONTROLLERS_SHARED_HEADER
#define SCENE_GRAPH_CONTROLLERS_SHARED_HEADER

#include <stl/hash_map>
#include <stl/hash_set>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/stat_counter.h>
#include <xtl/trackable.h>
#include <xtl/visitor.h>

#include <math/utility.h>

#include <media/geometry/mesh.h>
#include <media/rms/binding.h>
#include <media/spine/animation_state.h>
#include <media/spine/skeleton.h>

#include <sg/dynamic_mesh.h>

#include <sg/controllers/acceleration_evaluators.h>
#include <sg/controllers/align_with_node.h>
#include <sg/controllers/look_to_node_point.h>
#include <sg/controllers/move_to_node_point.h>
#include <sg/controllers/resource_controller.h>
#include <sg/controllers/spine_animation.h>
#include <sg/controllers/spine_skeleton_visual_structure_builder.h>
#include <sg/controllers/water.h>

#endif
