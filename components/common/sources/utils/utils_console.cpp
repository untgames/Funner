#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/signal.h>

#include <common/component.h>
#include <common/console.h>
#include <common/singleton.h>
#include <common/strlib.h>

using namespace common;

namespace
{

const char* CONSOLE_HANDLERS_MASK = "common.console.*";

class ConsoleImpl
{
  public:
///Печать сообщений
    void Print (const char* message)
    {
      if (!message)
        throw xtl::make_null_argument_exception ("common::Console::Print", "message");

      LoadConsoleHandlers ();

      try
      {
        signal (message);
      }
      catch (...)
      {
      }
    }

    void VPrintf (const char* message, va_list list)
    {
      if (!message)
        throw xtl::make_null_argument_exception ("common::Console::VPrintf", "message");

      LoadConsoleHandlers ();

      try
      {
        Print (common::vformat (message, list).c_str ());
      }
      catch (...)
      {
      }
    }

///Регистрация обработчиков событий консоли
    xtl::connection RegisterEventHandler (const Console::EventHandler& handler)
    {
      return signal.connect (handler);
    }

  private:
    void LoadConsoleHandlers ()
    {
      static ComponentLoader console_handlers_loader (CONSOLE_HANDLERS_MASK);
    }

  private:
    typedef xtl::signal<void (const char*)> EventSignal;

  private:
    EventSignal signal;
};

/*
    Синглтон консоли
*/

typedef Singleton<ConsoleImpl> ConsoleSingleton;

}

/*
   Печать сообщений
*/

void Console::Print (const char* message)
{
  ConsoleSingleton::Instance ().Print (message);
}

void Console::Printf (const char* message, ...)
{
  va_list list;

  va_start (list, message);
  VPrintf  (message, list);
}

void Console::VPrintf (const char* message, va_list list)
{
  ConsoleSingleton::Instance ().VPrintf (message, list);
}

/*
   Регистрация обработчиков событий консоли
*/

xtl::connection Console::RegisterEventHandler (const Console::EventHandler& handler)
{
  return ConsoleSingleton::Instance ().RegisterEventHandler (handler);
}
