#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable.h>

#include <common/property_map.h>
#include <common/strlib.h>

#include <syslib/semaphore.h>
#include <syslib/thread.h>

#include <render/scene_render_server.h>

#include <render/scene/interchange/connection_manager.h>
#include <render/scene/interchange/context.h>
#include <render/scene/interchange/property_synchronizer.h>
#include <render/scene/interchange/render_thread.h>
#include <render/scene/interchange/serializer.h>

#include <shared/server.h>

namespace render
{

namespace scene
{

namespace server
{

using interchange::uint64;
using interchange::int64;
using interchange::uint32;
using interchange::int32;
using interchange::uint16;
using interchange::int16;
using interchange::uint8;
using interchange::int8;
using interchange::bool8;
using interchange::float32;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Внутренние команды сервера
///////////////////////////////////////////////////////////////////////////////////////////////////
enum InternalCommandId
{
  InternalCommandId_OnWindowAttached = interchange::CommandId_FirstUserDefined,
  InternalCommandId_OnWindowDetached,
  InternalCommandId_OnWindowSizeChanged,
  InternalCommandId_OnWindowViewportChanged,
  InternalCommandId_OnWindowHandleChanged,
  InternalCommandId_OnWindowPaint,
};

typedef interchange::Context<interchange::ServerToClientSerializer, interchange::ClientToServerDeserializer> Context;
typedef interchange::PropertyMapAutoWriter::Synchronizer                                                     PropertyMapSynchronizer;

#include "connection_acceptor.h"
#include "connection_state.h"
#include "connection.h"
#include "server_loopback_connection.h"
#include "client_window_manager.h"

}

}

}
