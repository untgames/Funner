#include "shared.h"

using namespace media::players;

namespace components
{

namespace openal_media_player
{

const char* TARGET_NAME = "low_level_openal";

/*
   Компонент проигрывания медиа
*/

class PlayerComponent
{
  public:
    //загрузка компонента
    PlayerComponent ()
    {
      StreamPlayerManager::RegisterPlayer (TARGET_NAME, &PlayerComponent::CreatePlayer);
    }

    static IStreamPlayer* CreatePlayer (const char* target_name, const char* source_name, const StreamPlayerManager::StreamEventHandler& handler)
    {
      static const char* METHOD_NAME = "sound::low_level_media_players::openal:PlayerComponent::CreatePlayer";

      if (!target_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

      if (!source_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "source_name");

      if (!xtl::xstrcmp (target_name, TARGET_NAME))
        return new sound::low_level_media_players::LowLevelPlayer<sound::low_level_media_players::DriverType_OpenAL> (source_name, handler);

      throw xtl::make_argument_exception (METHOD_NAME, "target_name", target_name);
    }
};

extern "C"
{

common::ComponentRegistrator<PlayerComponent> OpenALPlayer ("media.players.stream.openal");

}

}

}
