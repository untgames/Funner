#ifndef SOUND_LOW_LEVEL_MEDIA_PLAYERS_SHARED_HEADER
#define SOUND_LOW_LEVEL_MEDIA_PLAYERS_SHARED_HEADER

#include <xtl/common_exceptions.h>

#include <common/component.h>

#include <media/players/stream.h>

namespace sound
{

namespace low_level_media_players
{

///Создание проигрывателя
media::players::IStreamPlayer* create_stream_player (const char* target, const char* stream_name, const media::players::StreamPlayerManager::StreamEventHandler& handler);

}

}

#endif
