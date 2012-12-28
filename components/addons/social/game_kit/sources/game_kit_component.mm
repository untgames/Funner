#include "shared.h"

using namespace social;
using namespace social::game_kit;

namespace
{

const char* COMPONENT_NAME    = "social.sessions.game_kit"; //имя компонента
const char* SESSION_ID        = "GameKit";                  //идентификатор сессии
const char* SESSION_NAME_MASK = "GameKit";                  //маска имен сессии

}

/*
   Компонент сессии
*/

namespace components
{

namespace game_kit
{

class GameKitComponent
{
  public:
    GameKitComponent ()
    {
      if (GameKitSessionImpl::IsApiAvailable ())
        SessionManager::RegisterSession (SESSION_ID, SESSION_NAME_MASK, xtl::bind (&GameKitComponent::CreateSession, this, _1, _2));
    }

  private:
    ISessionManager* CreateSession (const char* session_name, const common::PropertyMap& config)
    {
      static const char* METHOD_NAME = "social::game_kit::GameKitComponent::CreateSession";

      if (xtl::xstrcmp (session_name, SESSION_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "session_name", session_name);

      GameKitSessionImpl::CheckUnknownProperties (METHOD_NAME, config, 0, 0);

      return new GameKitSessionImpl ();
    }
};

extern "C"
{

common::ComponentRegistrator<GameKitComponent> GameKitSession (COMPONENT_NAME);

}

}

}
