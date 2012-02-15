#ifndef MEDIALIB_ANIMATION_SHARED_HEADER
#define MEDIALIB_ANIMATION_SHARED_HEADER

#include <cmath>

#include <stl/auto_ptr.h>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>
#include <xtl/trackable.h>

#include <math/vector.h>
#include <math/matrix.h>
#include <math/quat.h>

#include <common/component.h>
#include <common/property_map.h>
#include <common/strlib.h>

#include <media/animation/animation_blender.h>
#include <media/animation/animation_library.h>
#include <media/animation/animation_state.h>
#include <media/animation/animation.h>
#include <media/animation/channel.h>
#include <media/animation/channel_blender.h>
#include <media/animation/event_track.h>
#include <media/animation/target_blender.h>

#include <media/resource_library.h>

#endif
