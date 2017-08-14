#ifndef RENDER_SCENE_CLIENT_IMPL_CORE_SHARED_HEADER
#define RENDER_SCENE_CLIENT_IMPL_CORE_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/array>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/trackable_ptr.h>

#include <common/property_map.h>
#include <common/strlib.h>
#include <common/time.h>

#include <media/image.h>

#include <syslib/condition.h>

#include <render/scene_render_client.h>

#include <render/scene/interchange/command_queue.h>
#include <render/scene/interchange/connection_manager.h>

#include <shared/client.h>
#include <shared/font_manager.h>
#include <shared/material_manager.h>
#include <shared/scene_manager.h>

namespace render
{

namespace scene
{

namespace client
{

#include "connection.h"
#include "render_target.h"
#include "renderable_view.h"

}

}

}

#endif
