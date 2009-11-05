#include "shared.h"

using namespace media::players;
using namespace media::players::iphone;

namespace
{

const char* MUSIC_TARGET_NAME                  = "music";
const char* VIDEO_DEFAULT_CONTROLS_TARGET_NAME = "video";
const char* VIDEO_NO_CONTROLS_TARGET_NAME      = "video_no_controls";
const char* VIDEO_VOLUME_CONTROLS_TARGET_NAME  = "video_volume_controls";
const char* VIDEO_ALL_CONTROLS_TARGET_NAME     = "video_all_controls";

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
      StreamPlayerManager::RegisterPlayer (VIDEO_DEFAULT_CONTROLS_TARGET_NAME, &PlayersComponent::CreatePlayer);
      StreamPlayerManager::RegisterPlayer (VIDEO_NO_CONTROLS_TARGET_NAME, &PlayersComponent::CreatePlayer);
      StreamPlayerManager::RegisterPlayer (VIDEO_VOLUME_CONTROLS_TARGET_NAME, &PlayersComponent::CreatePlayer);
      StreamPlayerManager::RegisterPlayer (VIDEO_ALL_CONTROLS_TARGET_NAME, &PlayersComponent::CreatePlayer);
    }

    static IStreamPlayer* CreatePlayer (const char* target_name, const char* source_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      static const char* METHOD_NAME = "media::players::iphone::PlayersComponent::CreatePlayer";

      if (!target_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

      if (!xtl::xstrcmp (target_name, MUSIC_TARGET_NAME))
        return create_music_player (source_name, &handler);
      else if (!xtl::xstrcmp (target_name, VIDEO_NO_CONTROLS_TARGET_NAME))
        return create_movie_player (source_name, &handler, VideoPlayerControlsType_NoControls);
      else if (!xtl::xstrcmp (target_name, VIDEO_VOLUME_CONTROLS_TARGET_NAME))
        return create_movie_player (source_name, &handler, VideoPlayerControlsType_VolumeControls);
      else if (!xtl::xstrcmp (target_name, VIDEO_ALL_CONTROLS_TARGET_NAME) || !xtl::xstrcmp (target_name, VIDEO_DEFAULT_CONTROLS_TARGET_NAME))
        return create_movie_player (source_name, &handler, VideoPlayerControlsType_AllControls);

      throw xtl::make_argument_exception (METHOD_NAME, "target_name", target_name);
    }
};

}

extern "C"
{

common::ComponentRegistrator<PlayersComponent> IPhonePlayers ("media.players.stream.iphone");

}
