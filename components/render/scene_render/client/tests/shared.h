#include <stdio.h>

#include <exception>

#include <stl/vector>

#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <common/property_map.h>
#include <common/strlib.h>

#include <render/scene_render_client.h>

#include <render/scene/interchange/connection_manager.h>

#include "../sources/shared.h"

using namespace render::scene::client;
