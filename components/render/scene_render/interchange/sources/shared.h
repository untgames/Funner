#ifndef RENDER_SCENE_INTERCHANGE_SHARED_HEADER
#define RENDER_SCENE_INTERCHANGE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/uninitialized_storage.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/shared_queue.h>

#include <render/scene/interchange/command_buffer.h>
#include <render/scene/interchange/command_queue.h>
#include <render/scene/interchange/connection_manager.h>

#endif
