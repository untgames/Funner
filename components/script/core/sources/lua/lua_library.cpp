#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

namespace
{

/*
    Константы
*/

const char*  GLOBAL_LIBRARY_NAME        = "global";
const char*  STATIC_LIBRARY_PREFIX      = "static.";
const size_t STATIC_LIBRARY_PREFIX_SIZE = strlen (STATIC_LIBRARY_PREFIX);

/*
    Функции передаваемые lua
*/

//диспетчер вызовов
int unsafe_invoke_dispatch (lua_State* state)
{
    //получение указателя на шлюз

  const Invoker* invoker      = reinterpret_cast<const Invoker*> (lua_touserdata (state, lua_upvalueindex (1)));
  Interpreter*   interpreter  = reinterpret_cast<Interpreter*> (lua_touserdata (state, lua_upvalueindex (2)));
  const char*    invoker_name = lua_tostring (state, lua_upvalueindex (3));

  if (!invoker || !interpreter || !invoker_name)
    raise<RuntimeException> ("script::lua::invoke_dispatch", "Bad invoker call (no up-values found)");

    //проверка количества переданных аргументов

//  if ((int)invoker->ArgumentsCount () > lua_gettop (state))
//    raise<StackException> ("script::lua::invoke_dispatch(%s)", "Arguments count mismatch (expected %u, got %u)", 
//                           invoker_name, invoker->ArgumentsCount (), lua_gettop (state));

    //вызов шлюза

  try
  {
    size_t results_count = (*invoker)(interpreter->Interpreter::Stack ());
    
    return results_count;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("script::lua::invoke_dispatch(\"%s\")", invoker_name);
    throw;
  }
  catch (xtl::bad_any_cast& exception)
  {
    raise<RuntimeException> (format ("script::lua::invoke_dispatch(\"%s\")", invoker_name).c_str (),
                             "%s: %s->%s", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
  }
  catch (std::exception& exception)
  {
    raise<RuntimeException> (format ("script::lua::invoke_dispatch(\"%s\")", invoker_name).c_str (), "%s", exception.what ());
  }

  return 0;
}

int invoke_dispatch (lua_State* state)
{
  return safe_call (state, &unsafe_invoke_dispatch);
}

//получение поля по имени
int unsafe_variant_get_field_common (lua_State* state)
{
  static const char* METHOD_NAME = "script::lua::variant_get_field_common";

  if (!lua_getmetatable (state, 1))
    raise<RuntimeException> (METHOD_NAME, "Bad '__index' call. Object isn't variant");

  lua_pushvalue (state, 2);
  lua_rawget    (state, -2);

  if (!lua_isnil (state, -1)) //поле с заданным именем найдено
    return 1;

    //пытаемся найти поле с префиксом get_

  lua_pop (state, 1); //удаляем результат предыдущего поиска
  lua_pushfstring (state, "get_%s", lua_tostring (state, 2));
  lua_rawget      (state, -2);

  if (lua_isnil (state, -1)) //свойство с указанным именем не найдено
    raise<UndefinedFunctionCallException> (METHOD_NAME, "Field '%s' not found", lua_tostring (state, 2));

  return 0;
}

int unsafe_variant_get_field (lua_State* state)
{
  if (unsafe_variant_get_field_common (state))
    return 1;

    //помещение аргумента вызова шлюза в стек

  lua_pushvalue (state, 1);
  lua_call      (state, 1, 1);

  return 1;  
}

int unsafe_variant_get_static_field (lua_State* state)
{
  if (unsafe_variant_get_field_common (state))
    return 1;

  lua_call (state, 0, 1);

  return 1;  
}

int variant_get_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_get_field);
}

int variant_get_static_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_get_static_field);
}

//установка значения поля
int unsafe_variant_set_field_common (lua_State* state)
{
  static const char* METHOD_NAME = "script::lua::variant_set_field_common";

  if (!lua_getmetatable (state, 1))
    raise<RuntimeException> (METHOD_NAME, "Bad '__newindex' call. Object isn't variant");    

  lua_pushvalue (state, 2);
  lua_rawget    (state, -2);
  
  if (!lua_isnil (state, -1)) //если существует поле с указанным именем
  {
    lua_pop       (state, 1);  //удаляем результат предыдущего поиска
    lua_pushvalue (state, 3);
    lua_rawset    (state, -3);
    
    return 1;
  }

    //пытаемся найти поле с префиксом set_

  lua_pop         (state, 1); //удаляем результат предыдущего поиска
  lua_pushfstring (state, "set_%s", lua_tostring (state, 2));
  lua_rawget      (state, -2);

  if (lua_isnil (state, -1)) //свойство с указанным именем не найдено
    raise<UndefinedFunctionCallException> (METHOD_NAME, "Field '%s' not found (or read-only)", lua_tostring (state, 2));
    
  return 0;
}

int unsafe_variant_set_field (lua_State* state)
{
  if (unsafe_variant_set_field_common (state))
    return 1;

    //помещение аргументов вызова шлюза в стек

  lua_pushvalue (state, 1);
  lua_pushvalue (state, 3);

  int top_index = lua_gettop (state) - 3;

  lua_call (state, 2, LUA_MULTRET);

  return lua_gettop (state) - top_index;
}

int unsafe_variant_set_static_field (lua_State* state)
{
  if (unsafe_variant_set_field_common (state))
    return 1;

    //помещение аргументов вызова шлюза в стек

  lua_pushvalue (state, 3);

  int top_index = lua_gettop (state) - 2;

  lua_call (state, 1, LUA_MULTRET);

  return lua_gettop (state) - top_index;
}

int variant_set_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_set_field);
}

int variant_set_static_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_set_static_field);
}

}

/*
    Конструкторы / деструктор
*/

Library::Library (Interpreter& in_interpreter, const char* name, InvokerRegistry& in_registry)
  : state (in_interpreter.State ()),
    interpreter (in_interpreter),
    registry (in_registry),
    table_name (name),
    is_global (table_name == GLOBAL_LIBRARY_NAME),
    is_static (table_name.compare (0, STATIC_LIBRARY_PREFIX_SIZE, STATIC_LIBRARY_PREFIX) == 0)
{
  static const char* METHOD_NAME = "script::lua::Library::Library";

    //регистрация обработчиков удаления пользовательских типов данных    

  static const luaL_reg common_meta_table [] = {
    {"__gc",       &variant_destroy},
    {"__tostring", &variant_tostring},
    {"__index",    is_static ? &variant_get_static_field : &variant_get_field},
    {"__newindex", is_static ? &variant_set_static_field : &variant_set_field},
    {0, 0}
  };  

  luaL_register (state, name, common_meta_table);
  lua_pushvalue (state, -1);
  lua_setfield  (state, LUA_REGISTRYINDEX, name); //регистрация метатаблицы
  
    //помещение имени библиотеки в таблицу (отладочная информация)
    
  lua_pushstring (state, "__library_name");
  lua_pushstring (state, name);
  lua_rawset     (state, -3);
  
  try
  {    
      //для статически линкуемой библиотеки - регистрируем переменную
      
    if (is_static)
    {
      if (!lua_checkstack (state, 1))
        raise<StackException> (METHOD_NAME, "Not enough stack space."
        "Attempt to push 1 item in stack with %u items (stack_size=%u)", lua_gettop (state), LUAI_MAXCSTACK);
        
      static const size_t BUFFER_SIZE = sizeof (int); 

      void* buffer = lua_newuserdata (state, BUFFER_SIZE);

      if (!buffer)
        raise<StackException> (METHOD_NAME, "Fail to allocate %u bytes from stack", BUFFER_SIZE);

      luaL_getmetatable (state, name);
      lua_setmetatable  (state, -2);

      new (buffer) int (0);

        //регистрация переменной
          //!!!сделать регистрацию вложенных имён!!!

      lua_setfield (state, LUA_GLOBALSINDEX, name + STATIC_LIBRARY_PREFIX_SIZE);
    }

      //регистрация шлюзов

    for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
      RegisterInvoker (registry.InvokerId (i), *i);

      //регистрация обработчиков событий

    on_register_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnRegisterInvoker,
      xtl::bind (&Library::RegisterInvoker, this, _2, _3));

    on_unregister_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnUnregisterInvoker,
      xtl::bind (&Library::UnregisterInvoker, this, _2));
  }
  catch (...)
  {
    Destroy ();

    throw;    
  }
}

Library::~Library ()
{
  Destroy ();
}

void Library::Destroy ()
{
     //unregister all invokers

  lua_pushnil   (state);
  lua_pushvalue (state, -1);
  lua_setfield  (state, LUA_REGISTRYINDEX, table_name.c_str ());
  lua_setfield  (state, LUA_GLOBALSINDEX, table_name.c_str ());
  
  if (is_static)
  {
    lua_pushnil  (state);  
    lua_setfield (state, LUA_GLOBALSINDEX, table_name.c_str () + STATIC_LIBRARY_PREFIX_SIZE);
  }
}

/*
    Регистрация/удаление шлюзов
*/

void Library::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  if (!strcmp (invoker_name, "__index") || !strcmp (invoker_name, "__newindex") || !strcmp (invoker_name, "__gc"))
    return; //регистрация шлюзов с указанными имена запрещена

  luaL_getmetatable     (state, table_name.c_str ());
  lua_pushlightuserdata (state, &invoker);
  lua_pushlightuserdata (state, &interpreter);
  lua_pushfstring       (state, "%s.%s", table_name.c_str (), invoker_name);
  lua_pushcclosure      (state, &invoke_dispatch, 3);
  
  if (is_global)
  {
    lua_pushvalue (state, -1);
    lua_setfield  (state, -3, invoker_name);
    lua_setglobal (state, invoker_name);
  }
  else
  {
    lua_setfield (state, -2, invoker_name);
  }

  lua_pop (state, 1);
}

void Library::UnregisterInvoker (const char* invoker_name)
{
  luaL_getmetatable (state, table_name.c_str ());
  lua_pushnil       (state);
  lua_setfield      (state, -2, invoker_name);

  if (is_global)
  {
    lua_pushnil   (state);
    lua_setglobal (state, invoker_name);
  }

  lua_pop (state, 1);
}
