#include <stdio.h>

#include <exception>

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>

#include <math/utility.h>

#include <common/log.h>
#include <common/property_map.h>
#include <common/time.h>

#include <media/font_library.h>
#include <media/particles/particle_system_library.h>
#include <media/spine/animation_state.h>
#include <media/spine/animation_state_data.h>
#include <media/spine/skeleton.h>
#include <media/spine/skeleton_data.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <sg/camera.h>
#include <sg/light.h>
#include <sg/line.h>
#include <sg/mesh.h>
#include <sg/page_curl.h>
#include <sg/particle_emitter.h>
#include <sg/scissor.h>
#include <sg/scene.h>
#include <sg/spine_skeleton.h>
#include <sg/sprite.h>
#include <sg/text.h>

#include <sg/controllers/spine_animation.h>
#include <sg/controllers/spine_skeleton_visual_structure_builder.h>

#include <render/scene_render_client.h>
#include <render/scene_render_server.h>

using namespace render;
