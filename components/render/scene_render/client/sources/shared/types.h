#ifndef RENDER_SCENE_CLIENT_TYPES_SHARED_HEADER
#define RENDER_SCENE_CLIENT_TYPES_SHARED_HEADER

#include <render/scene/interchange/context.h>
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
using interchange::NodeType;

typedef interchange::Context<interchange::ClientToServerSerializer, interchange::ServerToClientDeserializer> Context;

}

}

}

#endif
