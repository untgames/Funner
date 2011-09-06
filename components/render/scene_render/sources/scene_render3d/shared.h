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

//forwards
class Render;
class View;
class Scene;

//pointers
typedef xtl::intrusive_ptr<Render> RenderPtr;
typedef xtl::intrusive_ptr<View>   ViewPtr;
typedef xtl::intrusive_ptr<Scene>  ScenePtr;

//includes
#include "view.h"
#include "render.h"
#include "log.h"

}

}

#endif
