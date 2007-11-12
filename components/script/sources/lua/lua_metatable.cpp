#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

/*
    Конструкторы / деструктор
*/

Metatable::Metatable (lua_State* in_state, const char* name, InvokerRegistry& in_registry)
  : registry (in_registry), state (in_state), table_name (name)
{
    //создание новой метатаблицы

  if (!luaL_newmetatable (state, name))
    Raise<RuntimeException> ("script::lua::Metatable::Metatable", "Error at create metatable");

    //регистрация обработчиков удаления пользовательских типов данных

  static const luaL_reg common_meta_table [] = {
    {"__gc", &destroy_object},
    {0, 0}
  };  

  luaI_openlib (state, 0, common_meta_table, 0);    
      
    //установка индексной таблицы (фактически поле __index метатаблицы указывает на саму метатаблицу)
  
  lua_pushstring (state, "__index");
  lua_pushvalue  (state, -2);
  lua_settable   (state, -3);
  lua_pop        (state, 1);

  try
  {
      //регистрация шлюзов

    for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
      RegisterInvoker (registry.InvokerId (i), *i);

      //регистрация обработчиков событий

    on_register_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnRegisterInvoker,
      xtl::bind (&Metatable::RegisterInvoker, this, _2, _3));

    on_unregister_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnUnregisterInvoker,
      xtl::bind (&Metatable::UnregisterInvoker, this, _2));  
  }
  catch (...)
  {
    Destroy ();

    throw;    
  }
}

Metatable::~Metatable ()
{
  Destroy ();
}

void Metatable::Destroy ()
{
  lua_pushnil  (state);
  lua_setfield (state, LUA_REGISTRYINDEX, table_name.c_str ());
}

/*
    Регистрация/удаление шлюзов
*/

void Metatable::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  luaL_getmetatable     (state, table_name.c_str ()); //optimize!!
  lua_pushlightuserdata (state, &invoker);
  lua_pushstring        (state, invoker_name);
  lua_pushcclosure      (state, &invoke_dispatch, 2);
  lua_setfield          (state, -2, invoker_name);
  lua_pop               (state, 1);
  
  printf ("register invoker '%s'::'%s' %d\n", table_name.c_str (), invoker_name, lua_gettop (state));
}

void Metatable::UnregisterInvoker (const char* invoker_name)
{
  printf ("unregister '%s'\n", invoker_name);

  luaL_getmetatable (state, table_name.c_str ()); //optimize!!
  lua_pushnil       (state);
  lua_setfield      (state, -2, invoker_name);
}
