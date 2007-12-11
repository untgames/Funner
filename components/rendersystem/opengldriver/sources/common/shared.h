#ifndef RENDER_GL_DRIVER_COMMON_SHARED_HEADER
#define RENDER_GL_DRIVER_COMMON_SHARED_HEADER

#include <memory.h>
#include <stdarg.h>

#include <render/low_level/device.h>
#include <render/low_level/debug.h>

#include <shared/property_list.h>
#include <shared/context_object.h>
#include <shared/buffer.h>
#include <shared/platform/context.h>

#include <stl/string>
#include <stl/vector>
#include <stl/hash_map>

#include <xtl/reference_counter.h>
#include <xtl/bind.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/intrusive_ptr.h>

#include <common/exception.h>
#include <common/singleton.h>
#include <common/strlib.h>

#endif
