#include <cstdio>
#include <exception>

#include <xtl/reference_counter.h>
#include <xtl/iterator.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/common_exceptions.h>

#include <sg/camera.h>
#include <sg/scene.h>

#include <render/desktop.h>
#include <render/scene_render.h>
#include <render/custom_render.h>
#include <render/mid_level/renderer.h>

using namespace render;
using namespace scene_graph;
