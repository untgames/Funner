#ifndef MEDIALIB_PLAYER_SHARED_HEADER
#define MEDIALIB_PLAYER_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>

#include <common/component.h>
#include <common/singleton.h>

#include <media/player/play_list.h>
#include <media/player/player.h>
#include <media/player/stream.h>

namespace media
{

namespace player
{

///Создание проигрывателя
IStreamPlayer* create_player (const char* target, const char* stream_name, const StreamManager::StreamEventHandler& handler);

}

}

#endif
