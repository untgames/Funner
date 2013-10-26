#include <stl/auto_ptr.h>
#include <stl/hash_map>

#include <xtl/array>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable_ptr.h>

#include <common/property_map.h>
#include <common/strlib.h>
#include <common/time.h>

#include <media/image.h>

#include <syslib/condition.h>

#include <sg/scene.h>

#include <render/scene_render_client.h>

#include <render/scene/interchange/command_queue.h>
#include <render/scene/interchange/connection_manager.h>
#include <render/scene/interchange/context.h>
#include <render/scene/interchange/property_synchronizer.h>
#include <render/scene/interchange/serializer.h>

namespace render
{

namespace scene
{

namespace client
{

using interchange::int64;
using interchange::uint64;
using interchange::int32;
using interchange::uint32;
using interchange::int16;
using interchange::uint16;
using interchange::int8;
using interchange::uint8;
using interchange::float32;
using interchange::bool8;
using interchange::object_id_t;

typedef interchange::Context<interchange::ClientToServerSerializer, interchange::ServerToClientDeserializer> Context;
typedef interchange::PropertyMapAutoWriter::Synchronizer                                                     PropertyMapSynchronizer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип синхронизируемого объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ObjectType
{
  ObjectType_Viewport,
  ObjectType_RenderTarget,

  ObjectType_Num
};

#include "client.h"
#include "connection.h"
#include "render_target.h"
#include "renderable_view.h"
#include "scene.h"
#include "scene_manager.h"
#include "scene_object.h"

}

}

}
