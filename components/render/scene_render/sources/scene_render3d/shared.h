#ifndef RENDER_SCENE_RENDER3D_SHARED_HEADER
#define RENDER_SCENE_RENDER3D_SHARED_HEADER
               
#include <stl/algorithm>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <render/scene_render.h>

namespace render
{

namespace scene_render3d
{

#include "render.h"
#include "log.h"

}

}

#endif
