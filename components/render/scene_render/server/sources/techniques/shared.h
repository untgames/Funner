#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>

#include <common/parser.h>
#include <common/property_map.h>
#include <common/property_binding_map.h>
#include <common/strlib.h>

#include <math/utility.h>

#include <shared/camera.h>
#include <shared/render_manager.h>
#include <shared/scene_manager.h>
#include <shared/server.h>
#include <shared/technique_manager.h>

#include <shared/sg/collection_visitor.h>
#include <shared/sg/light.h>
#include <shared/sg/visual_model.h>

#include <render/manager.h>

namespace render
{

namespace scene
{

namespace server
{

//foward declarations
class BasicRenderer;

typedef xtl::intrusive_ptr<BasicRenderer> BasicRendererPtr;

#include "basic_renderer.h"
#include "shadow_map_renderer.h"

}

}

}
