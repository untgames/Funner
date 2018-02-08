#ifndef SCENE_GRAPH_CONTROLLERS_TESTS_SHARED_HEADER
#define SCENE_GRAPH_CONTROLLERS_TESTS_SHARED_HEADER

#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <media/spine/animation_state.h>
#include <media/spine/animation_state_data.h>
#include <media/spine/skeleton.h>
#include <media/spine/skeleton_data.h>

#include <sg/scene.h>
#include <sg/spine_skeleton.h>

#include <sg/controllers/acceleration_evaluators.h>
#include <sg/controllers/align_with_node.h>
#include <sg/controllers/animation.h>
#include <sg/controllers/look_to_node_point.h>
#include <sg/controllers/move_to_node_point.h>
#include <sg/controllers/spine_animation.h>
#include <sg/controllers/spine_skeleton_visual_structure_builder.h>
#include <sg/controllers/water.h>

using namespace scene_graph;
using namespace scene_graph::controllers;

#endif
