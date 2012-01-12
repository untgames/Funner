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
        SessionManager::RegisterSession (SESSION_ID, SESSION_NAME_MASK, xtl::bind (&GameKitComponent::CreateSession, this));
    }

  private:
    ISessionManager* CreateSession ()
    {
      return new GameKitSessionImpl ();
    }
};

extern "C"
{

common::ComponentRegistrator<GameKitComponent> GameKitSession (COMPONENT_NAME);

}

}

}
