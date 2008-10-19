#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

namespace
{

/*
    Константы
*/

const char* GLOBAL_LIBRARY_NAME = "global";

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
    throw xtl::format_exception<RuntimeException> ("script::lua::invoke_dispatch", "Bad invoker call (no up-values found)");

    //проверка количества переданных аргументов

//  if ((int)invoker->ArgumentsCount () > lua_gettop (state))
//    throw xtl::format_exception<StackException> ("script::lua::invoke_dispatch(%s)", "Arguments count mismatch (expected %u, got %u)", 
//                           invoker_name, invoker->ArgumentsCount (), lua_gettop (state));

    //вызов шлюза

  try
  {
    size_t results_count = (*invoker)(interpreter->Interpreter::Stack ());
    
    return results_count;
  }
  catch (xtl::exception& exception)
  {
    stl::string stack;
    
    dump_stack (state, stack);
    
    exception.touch ("script::lua::invoke_dispatch(\"%s\", %s)", invoker_name, stack.c_str ());
    throw;
  }
  catch (xtl::bad_any_cast& exception)
  {
    stl::string stack;
    
    dump_stack (state, stack);    
    
    throw xtl::format_exception<RuntimeException> (format ("script::lua::invoke_dispatch(\"%s\", %s)", invoker_name).c_str (),
      "%s: %s->%s", exception.what (), exception.source_type ().name (), exception.target_type ().name (), stack.c_str ());
  }
  catch (std::exception& exception)
  {
    stl::string stack;
    
    dump_stack (state, stack);

    throw xtl::format_exception<RuntimeException> (format ("script::lua::invoke_dispatch(\"%s\", %s)", invoker_name).c_str (),
      "%s", exception.what (), stack.c_str ());
  }

  return 0;
}

int invoke_dispatch (lua_State* state)
{
  return safe_call (state, &unsafe_invoke_dispatch);
}

//получение поля по имени
int unsafe_variant_get_field (lua_State* state)
{
  static const char* METHOD_NAME = "script::lua::variant_get_field";

  if (!lua_getmetatable (state, 1))
    throw xtl::format_exception<RuntimeException> (METHOD_NAME, "Bad '__index' call. Object isn't variant");

  lua_pushvalue (state, 2);
  lua_rawget    (state, -2);

  if (!lua_isnil (state, -1)) //поле с заданным именем найдено
  {
    lua_remove (state, -2); //удаление метатаблицы
    return 1;
  }

    //пытаемся найти поле с префиксом get_

  lua_pop         (state, 1); //удаляем результат предыдущего поиска
  lua_pushfstring (state, "get_%s", lua_tostring (state, 2));
  lua_rawget      (state, -2);
  lua_remove      (state, -2); //удаление метатаблицы

  if (lua_isnil (state, -1)) //свойство с указанным именем не найдено  
  {
    lua_pop (state, 1); //удаление результата поиска

    throw xtl::format_exception<UndefinedFunctionCallException> (METHOD_NAME, "Field '%s' not found", lua_tostring (state, 2));
  }

  bool is_static_call = lua_isuserdata (state, 1) == 0;

    //помещение аргумента вызова шлюза в стек

  if (is_static_call)
  {
    lua_call (state, 0, 1);
  }
  else
  {
    lua_pushvalue (state, 1);
    lua_call      (state, 1, 1);      
  }
  
  return 1;
}

int variant_get_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_get_field);
}

//установка значения поля
int unsafe_variant_set_field (lua_State* state)
{
  static const char* METHOD_NAME = "script::lua::variant_set_field";  

  if (!lua_getmetatable (state, 1))
    throw xtl::format_exception<RuntimeException> (METHOD_NAME, "Bad '__newindex' call. Object isn't variant");            

    //пытаемся найти поле с префиксом set_

  lua_pushfstring (state, "set_%s", lua_tostring (state, 2));
  lua_rawget      (state, -2);

  if (lua_isnil (state, -1)) //свойство с указанным именем не найдено
  {
    lua_pop (state, 2); //удаление результата и метатаблицы
    
    if (!lua_istable (state, 3))
      throw xtl::format_exception<UndefinedFunctionCallException> (METHOD_NAME, "Field '%s' not found (or read-only)", lua_tostring (state, 2));
      
      //добавление вложенной таблицы

    lua_rawset (state, 1);

    return 0;
  }
  
    //удаление метатаблицы
    
  lua_remove (state, -2);
    
  bool is_static_call = lua_isuserdata (state, 1) == 0; //является ли вызов статическим    
  int  args_count     = is_static_call ? 1 : 2;

    //помещение аргументов вызова шлюза в стек

  if (!is_static_call)
    lua_pushvalue (state, 1);

  lua_pushvalue (state, 3);

  int top_index = lua_gettop (state) - args_count - 1;

  lua_call (state, args_count, LUA_MULTRET);

  return lua_gettop (state) - top_index;
}

int variant_set_field (lua_State* state)
{
  return safe_call (state, &unsafe_variant_set_field);
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
    is_global (table_name == GLOBAL_LIBRARY_NAME)
{
    //регистрация обработчиков удаления пользовательских типов данных    

  static const luaL_reg common_meta_table [] = {
    {"__gc",       &variant_destroy},
    {"__tostring", &variant_tostring},
    {"__index",    &variant_get_field},
    {"__newindex", &variant_set_field},
    {0, 0}
  };  
  
  luaL_register    (state, name, common_meta_table);
  lua_pushstring   (state, name);
  lua_pushvalue    (state, -2);
  lua_rawset       (state, LUA_REGISTRYINDEX); //регистрация метатаблицы
  lua_pushvalue    (state, -1);
  lua_setmetatable (state, -1);

    //помещение имени библиотеки в таблицу (отладочная информация)

  lua_pushstring (state, "__library_name");
  lua_pushstring (state, name);
  lua_rawset     (state, -3);
  lua_pop        (state, 1); //удаление таблицы из стека  
  
  try
  {    
      //регистрация шлюзов

    for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
      RegisterInvoker (registry.InvokerId (i), *i);

      //регистрация обработчиков событий

    on_register_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnRegisterInvoker,
      xtl::bind (&Library::RegisterInvoker, this, _2, _3));

    on_unregister_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnUnregisterInvoker,
      xtl::bind (&Library::UnregisterInvoker, this, _2));
  }
  catch (xtl::exception& exception)
  {
    Destroy ();
    
    exception.touch ("script::lua::Library::Library");
    
    throw;
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
}

/*
    Регистрация/удаление шлюзов
*/

void Library::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  if (!strcmp (invoker_name, "__index") || !strcmp (invoker_name, "__newindex") || !strcmp (invoker_name, "__gc"))
    return; //регистрация шлюзов с указанными имена запрещена

  luaL_getmetatable     (state, table_name.c_str ());  
  lua_pushstring        (state, invoker_name);
  lua_pushlightuserdata (state, &invoker);
  lua_pushlightuserdata (state, &interpreter);
  lua_pushfstring       (state, "%s.%s", table_name.c_str (), invoker_name);
  lua_pushcclosure      (state, &invoke_dispatch, 3);
  
  if (is_global)
  {
    lua_pushvalue (state, -2);
    lua_pushvalue (state, -1);
    lua_rawset    (state, -5);
    lua_rawset    (state, LUA_GLOBALSINDEX);
  }
  else
  {
    lua_rawset (state, -3);
  }

  lua_pop (state, 1);
}

void Library::UnregisterInvoker (const char* invoker_name)
{
  luaL_getmetatable (state, table_name.c_str ());
  lua_pushstring    (state, invoker_name);
  lua_pushnil       (state);
  lua_rawset        (state, -3);

  if (is_global)
  {
    lua_pushnil   (state);
    lua_setglobal (state, invoker_name);
  }

  lua_pop (state, 1);
}
