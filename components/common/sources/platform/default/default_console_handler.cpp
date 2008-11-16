#include "shared.h"

namespace
{

/*
     омпонент печатающий сообщени€ консоли
*/

class DefaultConsoleHandlerComponent
{
  public:
    DefaultConsoleHandlerComponent ()
    {
      event_connection = common::Console::RegisterEventHandler (xtl::bind (&DefaultConsoleHandlerComponent::EventHandler, this, _1));
    }

  private:
    void EventHandler (const char* event)
    {
      printf ("%s", event);
    }

  private:
    xtl::auto_connection event_connection; //соединение обработки сообщений консоли
};

}

extern "C"
{

common::ComponentRegistrator<DefaultConsoleHandlerComponent> DefaultConsoleHandler ("common.console.DefaultConsoleHandler");

}
