#ifndef ENGINE_API_SHARED_HEADER
#define ENGINE_API_SHARED_HEADER

#include <cstdio>

#include <stl/algorithm>
#include <stl/auto_ptr.h>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/string.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

#include <engine/engine.h>

namespace engine
{

IWindow* create_window (const char* name);

}

#endif
