#ifndef ENGINE_SUBSYSTEMS_COMMON_SHARED_HEADER
#define ENGINE_SUBSYSTEMS_COMMON_SHARED_HEADER

#include <climits>

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/console.h>
#include <common/file.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/strlib.h>
#include <common/time.h>

#include <syslib/application.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

#include <media/rms/manager.h>
#include <media/rms/server.h>

#include <physics/manager.h>

#endif
