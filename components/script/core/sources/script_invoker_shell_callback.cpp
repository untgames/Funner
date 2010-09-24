#include "shared.h"

using namespace script;

namespace
{

class ShellCallbackHandler
{
  public:
    ShellCallbackHandler (IInterpreter* in_interpreter) : interpreter (in_interpreter) {}
  
    void operator () (const char* command) const
    {
      try
      {
        if (!interpreter)
          throw xtl::format_operation_exception ("", "Interpreter has already destroyed");

        if (!command)
          throw xtl::make_null_argument_exception ("", "command");

        interpreter->DoCommands (command, command, strlen (command));
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("script::ShellCallbackHandler::operator ()");
        throw;
      }
    }

  private:
    typedef xtl::trackable_ptr<IInterpreter> InterpreterPtr;

  private:
    InterpreterPtr interpreter;
};

struct ShellCallbackInvoker
{
  size_t operator () (IStack& stack)
  { 
    xtl::function<void (const char*)> result = ShellCallbackHandler (&stack.Interpreter ());

    stack.Push (xtl::make_ref_any (result));

    return 1;
  }
};

}

/*
    Создание шлюза для функции обратного вызова функций шлюза
*/

namespace script
{

Invoker make_shell_callback_invoker ()
{
  return ShellCallbackInvoker ();
}

}
