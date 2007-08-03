#include <common/file.h>
#include <common/strlib.h>
#include <stl/string>
#include <stl/hash_map>
#include <xtl/function.h>

#include "shared.h"

using namespace common;
using namespace script::lua;

namespace
{

const char*  INVOKE_DISPATCH_NAME = "___recall"; //имя функции диспетчеризации в lua
const char*  IMPL_NAME            = "___impl";   //имя переменной, содержащей указатель на реализацию lua
const size_t IMPL_ID              = 0xDEADBEEF;  //идентификатор реализации lua

}

/*
    Описание реализации окружения Lua
*/

struct InvokerData
{
  detail::Invoker* invoker;
  stl::string      function;
};

//карта обработчиков вызова
typedef stl::hash_map<stl::hash_key<const char*>, InvokerData> InvokerMap;

namespace script
{

namespace lua
{

struct EnvironmentImpl
{
  size_t       id;           //идентификатор реализации
  lua_State*   state;        //состояние Lua
  InvokerMap   invokers;     //обработчики вызова
  stl::string  name;         //имя среды    
};

}

}

/*
    Вспомогательные функции
*/

namespace
{

//диспетчер вызовов
int invoke_dispatch (lua_State* state)
{
  try
  {
    lua_getglobal (state, IMPL_NAME);

    EnvironmentImpl* this_impl = reinterpret_cast<EnvironmentImpl*> (lua_touserdata (state, -1));
    
      //проверка корректности lua-переменной '__impl'
    
    if (!this_impl || this_impl->id != IMPL_ID)
      Raise<RuntimeException> ("script::lua::invoke_dispatch", "Wrong '__impl' variable");

    const char* function_name = lua_tostring (state, 1);
    
      //проверка наличия функции function_name

    if (!function_name)
      Raise<RuntimeException> ("script::lua::invoke_dispatch", "Null function name at call function '__recall'");

    InvokerMap::iterator iter = this_impl->invokers.find (function_name);

    if (iter == this_impl->invokers.end ())
      Raise<RuntimeException> ("script::lua::invoke_dispatch", "Attempt to call unregistered function '%s'", function_name);

      //собственно вызов функции

    return (*iter->second.invoker) (detail::Stack (state));
  }
  catch (std::exception& exception)
  {
    //очищаем стек
    //кладём текст исключения в стек
    lua_error (state);
  }
  catch (...)
  {
    //очищаем стек
    //кладём текст исключения в стек
    lua_error (state);
  }
  
  return 0;
}

//функция обработки ошибок lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "Lua internal error (lua calls 'lua_atpanic' function)");

  return 0;
}

//функция вызываемая уборщиком мусора при удалении объектов пользовательского типа данных
int user_data_destroyer (lua_State* state)
{
  detail::IUserData* user_data = reinterpret_cast<detail::IUserData*> (luaL_checkudata (state, -1, USER_DATA_TAG));
  
  if (!user_data)
    return 0;

  stl::destroy (user_data);

  return 0;
}

}


Environment::Environment ()
  : impl (new EnvironmentImpl)
{
  try
  {
    impl->id = IMPL_ID;

      //инициализация состояния lua

    impl->state = lua_open ();

    if (!impl->state)
      Raise<ScriptException> ("script::lua::Environment::Environment", "Can't create lua state");

      //убрать!!!!!!!
    
    luaL_openlibs (impl->state);
    
      //регистрация обработчиков удаления пользовательских типов данных    
      
    static const luaL_reg user_data_meta_table [] = {{"__gc", &user_data_destroyer}, {0,0}};    

    luaL_newmetatable (impl->state, USER_DATA_TAG);
    luaL_openlib      (impl->state, 0, user_data_meta_table, 0);
    
      //регистрация функции обработки ошибок

    lua_atpanic (impl->state, &error_handler);

      //регистрация диспетчера вызовов
    
    lua_pushcfunction (impl->state, &invoke_dispatch);
    lua_setglobal     (impl->state, INVOKE_DISPATCH_NAME);

    lua_pushlightuserdata (impl->state, impl);
    lua_setglobal         (impl->state, IMPL_NAME);  
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

/*void Environment::Restart ()
{
  lua_close (impl->state);

//
//   Инициализация (!!повтор!!)
//
  impl->state = lua_open ();

  if (!impl->state)
    Raise<ScriptException> ("script::lua::Environment::Environment", "Can't create lua state");

    //убрать!!!!!!!
  
  luaL_openlibs (impl->state);
  
    //регистрация обработчиков удаления пользовательских типов данных    
    
  static const luaL_reg user_data_meta_table [] = {{"__gc", &user_data_destroyer}, {0,0}};    

  luaL_newmetatable (impl->state, USER_DATA_TAG);
  luaL_openlib      (impl->state, 0, user_data_meta_table, 0);
  
    //регистрация функции обработки ошибок

  lua_atpanic (impl->state, &error_handler);

    //регистрация диспетчера вызовов
  
  lua_pushcfunction (impl->state, &invoke_dispatch);
  lua_setglobal     (impl->state, INVOKE_DISPATCH_NAME);

  lua_pushlightuserdata (impl->state, impl);
  lua_setglobal         (impl->state, IMPL_NAME);  

//
//   Регистрация функций
//
  for (InvokerMap::iterator iter=impl->invokers.begin (); iter!=impl->invokers.end (); ++iter)
    DoString (iter->second.function.c_str());
}*/

Environment::~Environment ()
{
  UnregisterAllFunctions ();

  lua_close (impl->state);

  delete impl;
}

/*
    Имя среды
*/

const char* Environment::Name () const
{
  return impl->name.c_str ();
}

void Environment::Rename (const char* new_name)
{
  if (!new_name)
    RaiseNullArgument ("script::lua::Environment::Rename", "new_name");

  impl->name = new_name;
}

/*
    Стек Lua-C
*/

detail::Stack Environment::Stack () const
{
  return impl->state;
}

/*
    Регистрация функций
*/

void Environment::RegisterFunctionCore (const char* name, detail::Invoker* invoker, size_t arguments_count)
{
  try
  {
    if (!name)
      RaiseNullArgument ("script::lua::Environment::RegisterFunction", "name");

    if (!invoker)
      RaiseNullArgument ("script::lua::Environment::RegisterFunction", "invoker");
      
      //генерация функции-оболочки, вызывающей диспетчер обработки вызовов

    stl::string args;
    args.reserve (8 * arguments_count);

    for (size_t i = 0; i < arguments_count; i++)
    {
      char arg_buf [8];

      sprintf (&arg_buf[0], "arg%u", i);
      args += &arg_buf[0];
      if (i < arguments_count - 1)
        args += ", ";
    }

    stl::string generated_function ("\nfunction ");

    generated_function.reserve (64 + args.length () * 2 + strlen (name) * 2 + strlen (INVOKE_DISPATCH_NAME));
   
    generated_function += name;
    generated_function += " (";
    generated_function += args;
    generated_function += ")\n return ";
    generated_function += INVOKE_DISPATCH_NAME;
    generated_function += " (\"";
    generated_function += name;
    generated_function += "\"";
    
    if (arguments_count)
    {
      generated_function += ", ";
      generated_function += args;
    }
    
    generated_function += ")\n end";

      //добавление обработчика в карту обработчиков
    InvokerData invoker_data = {invoker, generated_function};
    impl->invokers [name] = invoker_data;
    
    DoString (generated_function.c_str());
  }
  catch (...)
  {
    delete invoker;
    throw;
  }
}

void Environment::UnregisterFunction (const char* name) //no throw
{
  if (!name)
    return;
    
  InvokerMap::iterator iter = impl->invokers.find (name);
  
  if (iter == impl->invokers.end ())
    return;

  delete iter->second.invoker;

  impl->invokers.erase (iter);
}

void Environment::UnregisterAllFunctions () //no throw
{
  for (InvokerMap::iterator iter=impl->invokers.begin (); iter!=impl->invokers.end (); ++iter)
    delete iter->second.invoker;

  impl->invokers.clear ();
}

bool Environment::HasFunction (const char* name) const //no throw
{
  if (!name)
    return false;

  lua_getglobal (impl->state, name);
  if (lua_isfunction (impl->state, -1))
    return true;
  return false;
}

/*
    Выполнение выражения
*/

namespace
{

//функция протоколирования по умолчанию
void default_log_handler (const char*)
{
}

//временный буфер
struct Buffer
{
  Buffer (size_t size) : data (::operator new (size)) {}
  ~Buffer () { ::operator delete (data); }

  void* data;
};

}

bool Environment::DoString (const char* expression, const LogFunc& log)
{
  if (!expression)
    RaiseNullArgument ("script::lua::Environment::DoString", "expression");

  if (luaL_dostring (impl->state, expression))
  {
    log (lua_tostring (impl->state, -1));
    return false;
  }

  return true;
}

bool Environment::DoString (const char* expression)
{
  return DoString (expression, &default_log_handler);
}

bool Environment::DoBuffer (const char* name, const void* buffer, size_t buffer_size, const LogFunc& log)
{
  if (luaL_loadbuffer (impl->state, (const char*)buffer, buffer_size, name))
  {
    log (lua_tostring (impl->state, -1));

    return false;
  }
  
  if (lua_pcall(impl->state, 0, LUA_MULTRET, 0))
  {
    log (lua_tostring (impl->state, -1));

    return false;
  }

  return true;
}

bool Environment::DoBuffer (const char* name, const void* buffer, size_t buffer_size)
{
  return DoBuffer (name, buffer, buffer_size, &default_log_handler);
}

bool Environment::DoFile (const char* file_name, const LogFunc& log)
{
  if (!file_name)
    RaiseNullArgument ("script::lua::Environment::DoFile", "file_name");

  try
  {
    InputFile in_file (file_name);
    size_t    file_size = in_file.Size ();
    Buffer    buffer (file_size);

    in_file.Read (buffer.data, file_size);

    return DoBuffer (file_name, buffer.data, file_size, log);
  }
  catch (std::exception& exception)
  {
    log (common::format ("exception: %s", exception.what ()).c_str ());
    return false;    
  }
  catch (...)
  {
    log ("unknown exception");
    return false;
  }
}

bool Environment::DoFile (const char* file_name)
{
  return DoFile (file_name, &default_log_handler);
}

/*
    Вызов функци lua
*/

void Environment::InvokeCore (size_t args_count, size_t results_count) const
{
  if (lua_pcall (impl->state, args_count, results_count, 0))
    Raise <Exception> ("script::lua::Environment::Invoke", "Error running function: '%s'", lua_tostring (impl->state, -1));
}
