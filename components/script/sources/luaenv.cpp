#include <common/exception.h>
#include <common/file.h>
#include <common/strlib.h>
#include <stl/string>
#include <stl/memory>
#include <stl/hash_map>
#include <script.h>
#include <xtl/function.h>

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

using namespace common;
using namespace stl;
using namespace script;
using namespace script::lua;
using namespace script::lua::detail;

const char* RECALL_NAME = "___recall";
const char* IMPL_NAME = "___impl";

struct Base
{
  public:
    Base ();
    ~Base ();

    lua_State*                  l_state;
};

struct Environment::Impl : public Base
{
  public:
    Impl ();
    ~Impl ();

    static int Recaller (lua_State* l_state);
    static int AtPanic  (lua_State* l_state) {Raise <Exception> ("LuaEnvImpl::AtPanic", "Lua at panic."); return 0;}

    detail::Stack               stack;
    hash_map <string, Invoker*> funcs;
    string                      str_name;
    Environment::DebugLogFunc   log_function;
};

static int DestroyIUserData (lua_State* state)
{
  IUserData* ptr = (IUserData*)lua_touserdata (state, -1);
  
  if (!ptr) 
    luaL_typerror (state, 1, "user_data");    

  stl::destroy (ptr);
  
  return 0;
}

static const luaL_reg iuser_data_meta_table [] = {
  {"__gc", DestroyIUserData},
  {0,0}
};

Base::Base ()
{
  l_state = lua_open ();
  if (!l_state)
    Raise <Exception> ("Impl::Impl", "Can't create lua state.");

  luaL_openlibs (l_state);

  luaL_newmetatable (l_state,"iuser_data");
  luaL_openlib      (l_state,0,iuser_data_meta_table,0);
}

Base::~Base ()
{
  lua_close (l_state);
}

Environment::Impl::Impl ()
  : Base (), stack (l_state)
{
  lua_atpanic           (l_state, &Impl::AtPanic);
  lua_pushcfunction     (l_state, &Impl::Recaller);
  lua_setglobal         (l_state, RECALL_NAME);
  lua_pushlightuserdata (l_state, this);
  lua_setglobal         (l_state, IMPL_NAME);
}

Environment::Impl::~Impl ()
{
  for (hash_map <string, Invoker*>::iterator i = funcs.begin (); i != funcs.end (); i++)
    delete i->second;
}

int Environment::Impl::Recaller (lua_State* l_state)
{
  lua_getglobal (l_state, IMPL_NAME);

  Environment::Impl* this_impl = (Environment::Impl*)lua_touserdata(l_state, -1);  
  hash_map<string, Invoker*>::iterator func = this_impl->funcs.find (string (lua_tostring (l_state, 1)));

  if (func == this_impl->funcs.end ())
    Raise <Exception> ("Impl::Recaller", "Calling unregistered function.");

  return (*(func->second)) (this_impl->stack);
}

Environment::Environment (const DebugLogFunc& debug_log)
  : impl (new Impl) 
{
  SetDebugLog (debug_log);
}

Environment::~Environment ()
{
  delete impl;
}

const Environment::DebugLogFunc& Environment::GetDebugLog ()
{
  return impl->log_function;
}

void Environment::SetDebugLog (const DebugLogFunc& new_log_function)
{
  impl->log_function = new_log_function; 
}

const char* Environment::Name () const
{
  return impl->str_name.c_str ();
}

void Environment::Rename (const char* new_name)
{
  impl->str_name = new_name;
}

detail::Stack& Environment::Stack ()
{
  return impl->stack;
}

const detail::Stack& Environment::Stack () const
{
  return impl->stack;
}

struct  ScriptExceptionTag;         //ошибка выполнения скрипта
typedef DerivedException<Exception,ScriptExceptionTag> ScriptException;

void RaiseScriptException (const char* source,const char* format,...)
{
  va_list list;

  va_start (list,format);

  VRaise<ScriptException> (source,format,list);
}


void Environment::DoString (const char* expression)
{
  try
  {
    if (luaL_dostring (impl->l_state, expression))
      RaiseScriptException ("EnvironmentImpl::DoString", "Lua error executing occured when expression '%s'", expression);
  }
  catch (std::exception& exception)
  {                                               
    lua_error (impl->l_state);
    impl->log_function (Name (), exception.what ()); 
    RaiseScriptException ("EnvironmentImpl::DoString", "Std exception '%s' occured when executing expression '%s'", exception.what (), expression);
  }
  catch (...)
  {
    lua_error (impl->l_state);
    impl->log_function (Name (), "Unknown exception occured."); 
    RaiseScriptException ("EnvironmentImpl::DoString", "Unknown exception occured when executing expression '%s'", expression);
  }
}

void Environment::DoFile (const char* file_name)
{
  InputFile in_file (file_name);
  char* buffer = new char [in_file.Size ()];

  in_file.Read (buffer, in_file.Size ());

  try
  {
    if (luaL_dostring (impl->l_state, buffer))
      RaiseScriptException ("EnvironmentImpl::DoFile", "Lua error occured when executing file '%s'", file_name);
  }
  catch (std::exception& exception)
  {                                               
    lua_error (impl->l_state);
    impl->log_function (Name (), exception.what ()); 
    RaiseScriptException ("EnvironmentImpl::DoFile", "Std exception '%s' occured when executing file '%s'", exception.what (), file_name);
  }
  catch (...)
  {
    lua_error (impl->l_state);
    impl->log_function (Name (), "Unknown exception have occured."); 
    RaiseScriptException ("EnvironmentImpl::DoFile", "Unknown exception occured when executing file '%s'", file_name);
  }
}

void Environment::Invoke (size_t args_count, size_t results_count)
{
  if (lua_pcall(impl->l_state, args_count, results_count, 0))
    Raise <Exception> ("invoke", "Error running function: '%s'", lua_tostring(impl->l_state, -1));
}

void Environment::RegisterFunction (const char* name, Invoker* invoker)
{
  if (!name || !invoker)
    RaiseNullArgument ("Environment::RegisterFunction", "name or invoker");

  impl->funcs.insert_pair (string (name), invoker);

  string args;
  args.reserve (8 * invoker->ArgCount ());

  for (size_t i = 0; i < invoker->ArgCount (); i++)
  {
    char arg_buf [8];

    sprintf (&arg_buf[0], "arg%u", i);
    args += &arg_buf[0];
    if (i < invoker->ArgCount () - 1)
      args += ", ";
  }

  string generated_function ("\nfunction ");

  generated_function.reserve (64 + args.length () * 2 + strlen (name) * 2 + strlen (RECALL_NAME));
 
  generated_function += name;
  generated_function += " (";
  generated_function += args;
  generated_function += ")\n return ";
  generated_function += RECALL_NAME;
  generated_function += " (\"";
  generated_function += name;
  generated_function += "\"";
  if (invoker->ArgCount ())
  {
    generated_function += ", ";
    generated_function += args;
  }
  generated_function += ")\n end";

  DoString (generated_function.c_str());
}

void Environment::Swap (Environment& lua)
{
  stl::swap (impl, lua.impl);
}

void script::lua::swap (Environment& env1, Environment& env2)
{
  env1.Swap (env2);
}

void script::lua::invoke (Environment& env, const char* fn_name)
{
  env.Stack().PushFunction(fn_name);

  env.Invoke (0, 0);
}

