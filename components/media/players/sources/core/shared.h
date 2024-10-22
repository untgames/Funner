#ifndef MEDIALIB_PLAYER_SHARED_HEADER
#define MEDIALIB_PLAYER_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/string.h>

#include <media/players/playlist.h>
#include <media/players/media_player.h>
#include <media/players/stream.h>

namespace media
{

namespace players
{

///Создание проигрывателя
IStreamPlayer* create_stream_player (const char* target, const char* stream_name, const StreamPlayerManager::StreamEventHandler& handler);

}

}

#endif
