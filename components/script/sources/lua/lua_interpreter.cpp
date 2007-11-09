#include <common/exception.h>
#include <xtl/iterator.h>
#include <xtl/bind.h>
#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;
using namespace xtl;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

const char*  INVOKE_DISPATCH_NAME = "___recall"; //имя функции диспетчеризации в lua
const char*  IMPL_NAME            = "___impl";   //имя переменной, содержащей указатель на реализацию lua
const size_t IMPL_ID              = 0xDEADBEEF;  //идентификатор реализации lua

//диспетчер вызовов
int invoke_dispatch (lua_State* state)
{
  try
  {
    lua_getglobal (state, IMPL_NAME);

    LuaInterpreter* this_impl = reinterpret_cast<LuaInterpreter*> (lua_touserdata (state, -1));
    
      //проверка корректности lua-переменной '__impl'
    
    if (!this_impl || this_impl->Id () != IMPL_ID)
      Raise<RuntimeException> ("script::lua::invoke_dispatch", "Wrong '__impl' variable");

    const char* function_name = lua_tostring (state, 1);
    
      //проверка наличия функции function_name

    if (!function_name)
      Raise<UndefinedFunctionCallException> ("script::lua::invoke_dispatch", "Null function name at call function '__recall'");

    const Invoker* invoker = this_impl->Registry ().Find (function_name);

    if (!invoker)
      Raise<UndefinedFunctionCallException> ("script::lua::invoke_dispatch", "Invoker not found for function %s.", function_name);

    (*invoker) (this_impl->Stack ());
  }
  catch (std::exception& exception)
  {
    lua_pop    (state, lua_gettop (state));
    luaL_error (state, "exception: %s", exception.what ());
  }
  catch (...)
  {
    lua_pop    (state, lua_gettop (state));
    luaL_error (state, "unknown exception");
  }
  
  return 0;
}

//функция обработки ошибок lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "Lua internal error '%s' (lua calls 'lua_atpanic' function)", lua_tostring (state, -1));

  return 0;
}

//функция вызываемая уборщиком мусора при удалении объектов пользовательского типа данных
int user_data_destroyer (lua_State* state)
{
  if (!luaL_checkudata (state, -1, USER_DATA_TAG))
    return 0;

  stl::destroy (lua_touserdata (state, -1));

  return 0;
}

void log_function (const char*)
{
}

}

/*
    Конструктор / деструктор
*/

LuaInterpreter::LuaInterpreter (const InvokerRegistry& in_registry)
  : registry (in_registry), 
    ref_count (1), 
    id (IMPL_ID),
    on_register_invoker_connection (registry.RegisterHandler (InvokerRegistryEvent_OnRegisterInvoker, bind (&LuaInterpreter::OnRegisterInvoker, this, _1, _2)))
{
    //инициализация состояния lua

  state = lua_open ();

  if (!state)
    Raise <InterpreterException> ("LuaInterpreter::LuaInterpreter", "Can't create lua state");

  luaL_openlibs (state);
  
    //регистрация обработчиков удаления пользовательских типов данных    
    
  static const luaL_reg user_data_meta_table [] = {{"__gc", &user_data_destroyer}, {0,0}};    

  luaL_newmetatable (state, USER_DATA_TAG);
  luaL_openlib      (state, 0, user_data_meta_table, 0);
  
    //регистрация функции обработки ошибок

  lua_atpanic (state, &error_handler);

    //регистрация диспетчера вызовов
  
  lua_pushcfunction (state, &invoke_dispatch);
  lua_setglobal     (state, INVOKE_DISPATCH_NAME);

  lua_pushlightuserdata (state, this);
  lua_setglobal         (state, IMPL_NAME);  

  stack.SetState (state);
  
  for (InvokerRegistry::Iterator i = ((InvokerRegistry)registry).CreateIterator (); i; ++i)
    RegisterFunction (registry.InvokerId (i));
}

LuaInterpreter::~LuaInterpreter ()
{
  lua_close (state);
}

/*
    Имя интерпретатора
*/

const char* LuaInterpreter::Name ()
{
  return LUA_RELEASE;
}

/*
    Стек аргументов
*/

IStack& LuaInterpreter::Stack ()
{
  return stack;
}

/*
    Проверка наличия функции
*/

bool LuaInterpreter::HasFunction (const char* name)
{
  if (!name)
    return false;

  lua_getglobal (state, name);
  if (lua_isfunction (state, -1))
    return true;
  return false;
}

/*
    Выполнение буфера интерпретации луа
*/

void LuaInterpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
  if (luaL_loadbuffer (state, (const char*)buffer, buffer_size, buffer_name))
  {
    log (lua_tostring (state, -1));

    return;
  }
    
  if (lua_pcall(state, 0, LUA_MULTRET, 0))
    log (lua_tostring (state, -1));
}

/*
    Вызов функции луа
*/

void LuaInterpreter::Invoke (size_t arguments_count, size_t results_count)
{
  if (lua_pcall (state, arguments_count, results_count, 0))
    Raise <RuntimeException> ("LuaInterpreter::Invoke", "Error running function: '%s'", lua_tostring (state, -1));
}

/*
    Подсчёт ссылок
*/

void LuaInterpreter::AddRef ()
{
  ref_count++;
}

void LuaInterpreter::Release ()
{
  if (!--ref_count)
    delete this;
}

/*
   Регистрация функций
*/

void LuaInterpreter::RegisterFunction (const char* function_name)
{
  if (!function_name)
    RaiseNullArgument ("script::lua::LuaInterpreter::RegisterFunction", "function_name");

    //генерация функции-оболочки, вызывающей диспетчер обработки вызовов

  stl::string generated_function ("\nfunction ");

  generated_function.reserve (64 + strlen (function_name) * 2 + strlen (INVOKE_DISPATCH_NAME));
 
  generated_function += function_name;
  generated_function += " ()\n return ";
  generated_function += INVOKE_DISPATCH_NAME;
  generated_function += " (\"";
  generated_function += function_name;
  generated_function += "\")\n end";

  DoCommands (function_name, generated_function.c_str(), generated_function.length (), &log_function);
}

void LuaInterpreter::OnRegisterInvoker (InvokerRegistryEvent, const char* invoker_name)
{
  RegisterFunction (invoker_name);
}

namespace script
{

/*
    Создание интерпретатора lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const InvokerRegistry& registry)
{
  return xtl::com_ptr<IInterpreter> (new LuaInterpreter (registry), false);
}

}
