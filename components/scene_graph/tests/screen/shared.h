#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/log.h>
#include <common/property_map.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/screen.h>

using namespace scene_graph;
