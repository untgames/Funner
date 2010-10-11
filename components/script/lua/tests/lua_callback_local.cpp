#include "shared.h"

using namespace script;

const char* lua_f = 
"function test(x)\n"
"  local function callback(y)\n"
"    print('x=' .. tostring (x) .. ' y=' .. tostring (y))\n"
"    return tostring (x) .. ' ' .. tostring (y)\n"
"  end\n"
"  if x == 2 then return EventHandler(function (y) return tostring (y * x) end) end\n"
"  if x == 3 then return EventHandler('@return \"bad result\"') end\n"
"  return EventHandler(callback)\n"
"end\n"
;

int main ()
{
  try
  {
    printf ("Results of lua_callback_local_test:\n");
    
    typedef xtl::function<const char* (int)> my_fn;

    Environment env;

    InvokerRegistry& registry = env.CreateLibrary ("global");

    Shell shell ("lua", env);

    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    registry.Register ("EventHandler", make_callback_invoker<const char* (int)> ());

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f));

    typedef xtl::function<const char* (int)> my_fn;
    
    stl::string internal_name;
       
    for (int i=1; i<=3; i++)
    {
      {
        my_fn fn = invoke<my_fn> (*interpreter, "test", i);
        
        typedef detail::callback_dispatcher<const char*> internal_dispatcher_type;
        
        internal_dispatcher_type* dispatcher = fn.target<internal_dispatcher_type> ();
        
        if (dispatcher) internal_name = dispatcher->symbol_name ();
        else
        {
          printf ("wrong dispatcher type\n");
          internal_name = "";
        }

        printf ("result: '%s'\n", fn (i*2));
      }

      invoke<void> (*interpreter, "collectgarbage", "collect");

      if (!interpreter->HasFunction (internal_name.c_str ())) printf ("symbol destroyed successfull\n");
      else                                                    printf ("symbol '%s' has not destroyed\n", internal_name.c_str ());
    }
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
