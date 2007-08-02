#include <common/file.h>
#include <common/strlib.h>
#include <stl/string>
#include <stl/hash_map>
#include <xtl/function.h>

#include "shared.h"

using namespace common;
using namespace script::lua;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

const char*  INVOKE_DISPATCH_NAME = "___recall"; //имя функции диспетчеризации в lua
const char*  IMPL_NAME            = "___impl";   //имя переменной, содержащей указатель на реализацию lua
const size_t IMPL_ID               = 0xDEADBEEF;  //идентификатор реализации lua

}

/*
    Environment::Impl
    Описание реализации окружения Lua
*/

//карта обработчиков вызова
typedef stl::hash_map<stl::hash_key<const char*>, detail::Invoker*> InvokerMap;

namespace script
{

namespace lua
{

struct EnvironmentImpl
{
  size_t               id;           //идентификатор реализации
  Environment&         environment;  //ссылка на владельца
  lua_State*           state;        //состояние Lua
  InvokerMap           invokers;     //обработчики вызова
  stl::string          name;         //имя среды
  Environment::LogFunc log_function; //функция протоколирования
  
  EnvironmentImpl (Environment& in_environment) : environment (in_environment) {}
  
  void LogError (const char* format, ...);
};

}

}

/*
    Вывод сообщения об ошибке
*/

void EnvironmentImpl::LogError (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);
  
  try
  {
    log_function (environment, common::vformat (format, list).c_str ());
  }
  catch (...)
  {
    //поглощаем все исключения, поскольку данная функция может быть вызвана из C-кода
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
  lua_getglobal (state, IMPL_NAME);

  EnvironmentImpl* this_impl = reinterpret_cast<EnvironmentImpl*> (lua_touserdata (state, -1));
  
    //проверка корректности lua-переменной '__impl'
  
  if (!this_impl || this_impl->id != IMPL_ID)  
    return 0; //к сожалению невозможно вывести никаких сообщений, предупрежджающих об ошибке
    
  const char* function_name = lua_tostring (state, 1);
  
    //проверка наличия функции function_name
  
  if (!function_name)
  {
    this_impl->LogError ("Null function name at call function '__recall'");
    return 0;
  }

  InvokerMap::iterator iter = this_impl->invokers.find (function_name);

  if (iter == this_impl->invokers.end ())
  {
    this_impl->LogError ("Attempt to call unregistered function '%s'", function_name);
    return 0;
  }

    //собственно вызов функции
    
  try
  {    
    detail::Stack stack (this_impl->state);
    
    return (*iter->second) (stack);
  }
  catch (std::exception& exception)
  {
    this_impl->LogError ("Exception at call function '%s': %s", function_name, exception.what ());
    lua_error (this_impl->state);
    //поглощаем все исключения, поскольку данная функция может быть вызвана из C-кода
  }
  catch (...)
  {
    this_impl->LogError ("Unknown exception at call function '%s'", function_name);
    lua_error (this_impl->state);
    //поглощаем все исключения, поскольку данная функция может быть вызвана из C-кода
  }
  
  return 0;
}

//функция обработки ошибок lua
int error_handler (lua_State* state)
{
  lua_getglobal (state, IMPL_NAME);

  EnvironmentImpl* this_impl = reinterpret_cast<EnvironmentImpl*> (lua_touserdata (state, -1));
  
    //проверка корректности lua-переменной '__impl'
  
  if (!this_impl || this_impl->id != IMPL_ID)  
    return 0; //к сожалению невозможно вывести никаких сообщений, предупрежджающих об ошибке
    
  this_impl->LogError ("Fatal error at parse lua script");
  
  return 0;
}

//функция протоколирования по умолчанию
void default_log_function (Environment&, const char* message)
{
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
  : impl (new EnvironmentImpl (*this))
{
  impl->id = IMPL_ID;

    //инициализация состояния lua

  impl->state = lua_open ();

  if (!impl->state)
  {
    delete impl;
    Raise<ScriptException> ("script::lua::Environment::Environment", "Can't create lua state");
  }

    //убрать!!!!!!!
  
  luaL_openlibs (impl->state);
  
    //регистрация обработчиков удаления пользовательских типов данных    
    
  static const luaL_reg user_data_meta_table [] = {{"__gc", &user_data_destroyer}, {0,0}};    

  luaL_newmetatable (impl->state, USER_DATA_TAG);
  luaL_openlib      (impl->state, 0, user_data_meta_table, 0);
  
    //регистрация функции обработки ошибок

  lua_atpanic (impl->state, &error_handler);

    //регистрация диспетчера вызовов
  
  lua_pushcfunction     (impl->state, &invoke_dispatch);
  lua_setglobal         (impl->state, INVOKE_DISPATCH_NAME);
  lua_pushlightuserdata (impl->state, impl); //сделать через полноценный user data с проверкой типа!!!
  lua_setglobal         (impl->state, IMPL_NAME);
  
    //установка функции протоколирования по умолчанию

  SetLogHandler (&default_log_function);
}

Environment::~Environment ()
{
  UnregisterAllFunctions ();

  lua_close (impl->state);

  delete impl;
}

/*
    Работа с функцией протоколирования
*/

const Environment::LogFunc& Environment::GetLogHandler ()
{
  return impl->log_function;
}

void Environment::SetLogHandler (const LogFunc& new_log_function)
{
  impl->log_function = new_log_function; 
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
      
      //добавление обработчика в карту обработчиков

    impl->invokers [name] = invoker;
    
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

  delete iter->second;

  impl->invokers.erase (iter);
}

void Environment::UnregisterAllFunctions () //no throw
{
  for (InvokerMap::iterator iter=impl->invokers.begin (); iter!=impl->invokers.end (); ++iter)
    delete iter->second;

  impl->invokers.clear ();
}

bool Environment::HasFunction (const char* name) const //no throw
{
  if (!name)
    return false;

  //сделать!!!!!!!проверку наличия функции в контексте луа (включая имена функций, в скриптах)
  return false;
}

/*
    Выполнение выражения
*/

void Environment::DoString (const char* expression, const char* name)
{
  if (!expression)
    RaiseNullArgument ("script::lua::Environment::DoString", "expression");

  if (!name)
    name = expression;
  
      //сделать dobuffer
  if (luaL_dostring (impl->state, expression))
    Raise<ScriptException> ("script::lua::Environment::DoString", "Lua error at parser expression '%s'", name);
}

void Environment::DoFile (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("script::lua::Environment::DoFile", "file_name");

  InputFile in_file (file_name);
  size_t    file_size = in_file.Size ();
  char*     buffer    = (char*)::operator new (file_size + 1);

  try
  {
    in_file.Read (buffer, file_size);

    buffer [file_size] = '\0';

    DoString (buffer, file_name);

    ::operator delete (buffer);
  }
  catch (...)
  {
    ::operator delete (buffer);
    throw;
  }
}

/*
    Вызов функци lua
*/

void Environment::InvokeCore (size_t args_count, size_t results_count) const
{
  if (lua_pcall (impl->state, args_count, results_count, 0))
    Raise <Exception> ("script::lua::Environment::Invoke", "Error running function: '%s'", lua_tostring (impl->state, -1));
}
