#include "shared.h"

using namespace media::players;
using namespace media::players::iphone;

namespace
{

const char* MUSIC_TARGET_NAME = "music";
const char* VIDEO_TARGET_NAME = "video";

/*
   Компонент проигрывания медиа
*/

class PlayersComponent
{
  public:
    //загрузка компонента
    PlayersComponent ()
    {
      StreamPlayerManager::RegisterPlayer (MUSIC_TARGET_NAME, &PlayersComponent::CreatePlayer);
      StreamPlayerManager::RegisterPlayer (VIDEO_TARGET_NAME, &PlayersComponent::CreatePlayer);
    }

    static IStreamPlayer* CreatePlayer (const char* target_name, const char* source_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      static const char* METHOD_NAME = "media::players::iphone::PlayersComponent::CreatePlayer";

      if (!target_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

      if (!xtl::xstrcmp (target_name, MUSIC_TARGET_NAME))
        return create_music_player (source_name, &handler);
      else if (!xtl::xstrcmp (target_name, VIDEO_TARGET_NAME))
        return create_movie_player (source_name, &handler);

      throw xtl::make_argument_exception (METHOD_NAME, "target_name", target_name);
    }
};

}

extern "C"
{

common::ComponentRegistrator<PlayersComponent> IPhonePlayers ("media.players.stream.iphone");

}
