#include "shared.h"

using namespace common;
using namespace script::lua;

namespace
{

//получение строки с местом вызова
stl::string get_lua_position (lua_State* state)
{
  stl::string buffer;  
  lua_Debug   debug;
  
  for (int level=0; ; level++)
  {
    if (!lua_getstack (state, level, &debug))
      break;
      
    lua_getinfo (state, "Sln", &debug);
    
    if (debug.currentline == -1)
      continue;
    
    if (debug.name) buffer += common::format ("\n    at lua-call %s(%d): function=\"%s\"", debug.short_src, debug.currentline, debug.name);
    else            buffer += common::format ("\n    at lua-call %s(%d)", debug.short_src, debug.currentline);
  }

  return buffer;
}

//функция, вызываемая сборщиком мусора при удалении объектов пользовательского типа данных
int unsafe_variant_destroy (lua_State* state)
{
  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, -1));

  if (variant && lua_getmetatable (state, -1))
  {
      //все пользовательские типы данных, хранимые в стеке, приводятся к xtl::any*. проверка совпадения метатаблиц не требуется
    
    lua_pop (state, 1);
    
    delete variant;
  }

  return 0;
}

//печать в строку состояния объекта пользовательского типа данных
int unsafe_variant_tostring (lua_State* state)
{
  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, -1));
  
  if (variant && lua_getmetatable (state, -1))
  {
    lua_pop (state, 1);
    
    stl::string buffer;

    to_string      (buffer, *variant);
    lua_pushstring (state, buffer.c_str ());

    return 1;
  }

  return 0;
}

}

namespace script
{

namespace lua
{

//безопасный вызов шлюзов
int safe_call (lua_State* state, int (*f)(lua_State*))
{
  try
  {
    return f (state);
  }
  catch (xtl::bad_any_cast& exception)
  {
    luaL_error (state, "%s: %s -> %s%s", exception.what (), exception.source_type ().name (), exception.target_type ().name (),
                get_lua_position (state).c_str ());
  }
  catch (std::exception& exception)
  {
    luaL_error (state, "%s%s", exception.what (), get_lua_position (state).c_str ());
  }
  catch (...)
  {
    luaL_error (state, "%s%s", lua_gettop (state) ? lua_tostring (state, -1) : "internal error", get_lua_position (state).c_str ());
  }

  return 0;
}

//удаление объекта
int variant_destroy (lua_State* state)
{
  return safe_call (state, &unsafe_variant_destroy);
}

//печать в строку состояния объекта пользовательского типа данных
int variant_tostring (lua_State* state)
{
  return safe_call (state, &unsafe_variant_tostring);
}

}

}
