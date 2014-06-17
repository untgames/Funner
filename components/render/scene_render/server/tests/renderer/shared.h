#include <stdio.h>

#include <exception>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <math/utility.h>

#include <common/log.h>
#include <common/property_map.h>
#include <common/time.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <sg/camera.h>
#include <sg/mesh.h>
#include <sg/page_curl.h>
#include <sg/scene.h>
#include <sg/sprite.h>

#include <render/scene_render_client.h>
#include <render/scene_render_server.h>

using namespace render;
