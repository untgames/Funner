#include "shared.h"

namespace
{

/*
    Компонент печатающий сообщения консоли
*/

class DefaultConsoleHandlerComponent
{
  public:
    DefaultConsoleHandlerComponent ()
    {
      on_print_connection = common::Console::RegisterEventHandler (common::ConsoleEvent_OnPrint, xtl::bind (&DefaultConsoleHandlerComponent::OnPrint, this, _1));
      on_print_line_connection = common::Console::RegisterEventHandler (common::ConsoleEvent_OnPrintLine, xtl::bind (&DefaultConsoleHandlerComponent::OnPrintLine, this));
    }

  private:
    void OnPrint (const char* event)
    {
      printf ("%s", event);
    }

    void OnPrintLine ()
    {
      fflush (stdout);
    }

  private:
    xtl::auto_connection on_print_connection;
    xtl::auto_connection on_print_line_connection;
};

}

extern "C"
{

common::ComponentRegistrator<DefaultConsoleHandlerComponent> DefaultConsoleHandler ("common.console.DefaultConsoleHandler");

}
