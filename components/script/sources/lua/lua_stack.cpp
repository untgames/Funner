#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

namespace script
{

namespace lua
{

//тэг пользовательских данных
const char* USER_DATA_TAG = "user_data";

}

}

/*
    Конструкторы
*/

LuaStack::LuaStack ()
  : state (0)
  {}

LuaStack::LuaStack (lua_State* in_state)
  : state (in_state)
  {}  
  
/*
    Состояние машины Lua
*/

void LuaStack::SetState (lua_State* in_state)
{
  state = in_state;
}

/*
    Количество аргументов в стеке
*/

size_t LuaStack::Size ()
{
  return lua_gettop (state) + 1;
}

/*
    Получение аргументов из стека
*/

namespace
{

//проверка наличия элемента в стеке
void check_item_index (lua_State* state, size_t index, const char* function_name)
{
  size_t stack_size = lua_gettop (state) + 1; 

  if (index >= stack_size)
    Raise<StackException> (function_name, "Attempt to get item #%u from stack with %u items", index, stack_size);
}

}

float LuaStack::GetFloat (size_t index)
{
  check_item_index (state, index, "script::lua::LuaStack::GetFloat");
  return (float)lua_tonumber (state, index);
}

int LuaStack::GetInteger (size_t index)
{
  check_item_index (state, index, "script::lua::LuaStack::GetInteger");
  return lua_tointeger (state, index);
}

void* LuaStack::GetPointer (size_t index)
{
  check_item_index (state, index, "script::lua::LuaStack::GetPointer");
  return lua_touserdata (state, index);
}

const char* LuaStack::GetString (size_t index)
{
  check_item_index (state, index, "script::lua::LuaStack::GetString");
  return lua_tostring (state, index);
}

const char* LuaStack::GetSymbol (size_t index)
{
  RaiseNotSupported ("script::lua::LuaStack::GetSymbol");
  
  return "";
}

xtl::any& LuaStack::GetVariant (size_t index)
{
    //проверка индекса
    
  check_item_index (state, index, "script::lua::LuaStack::GetVariant");

    //получение аргумента

  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, index));

    //получение мета-таблицы

  if (variant && lua_getmetatable (state, index))
  {
    lua_getfield (state, LUA_REGISTRYINDEX, USER_DATA_TAG); //кэшировать!!!!!
    
      //проверка совпадения типов
    
    if (lua_rawequal (state, -1, -2))
    {
      lua_pop (state, 2);

      return *variant;
    }
  }

    //если типы не совпали

  luaL_typerror (state, index, USER_DATA_TAG);

    //генерация исключения

  Raise<RuntimeException> ("script::LuaStack::GetVariant", "Item %u has wrong type (non user-data)", index);
  
  return *variant;
}

/*
    Помещение аргументов в стек
*/

namespace
{

//проверка возможности поместить в стек count аргументов 
void check_stack (lua_State* state, size_t count = 1)
{
  if (!lua_checkstack (state, count))
    Raise<StackException> ("script::lua::LuaStack::Push", "Not enough stack space."
    "Attempt to push %u items in stack with %u items (stack_size=%u)", count, lua_gettop (state), LUAI_MAXCSTACK);
}

}

void LuaStack::Push (float value)
{
  check_stack    (state);
  lua_pushnumber (state, value);
}        

void LuaStack::Push (int value)
{
  check_stack    (state);
  lua_pushinteger (state, value);
}

void LuaStack::Push (const char* string)
{
  if (!string)
    RaiseNullArgument ("script::lua::LuaStack::Push", "string");

  check_stack    (state);
  lua_pushstring (state, string);
}

void LuaStack::Push (void* value)
{
  check_stack           (state);
  lua_pushlightuserdata (state, value);
}

void LuaStack::PushSymbol (const char* symbol)
{
  if (!symbol)
    RaiseNullArgument ("script::lua::LuaStack::PushSymbol", "symbol");

  check_stack   (state);    
  lua_getglobal (state, symbol);
}

void LuaStack::Push (const xtl::any& object)
{
  static const size_t BUFFER_SIZE = sizeof (xtl::any);

  void* buffer = lua_newuserdata (state, BUFFER_SIZE);
  
  if (!buffer)
    Raise<StackException> ("script::lua::LuaStack::Push(const xtl::any&)", "Fail to allocate %u bytes from lua stack", BUFFER_SIZE);

  luaL_getmetatable (state, USER_DATA_TAG);
  lua_setmetatable  (state, -2);
  
  try
  {
    new (buffer) xtl::any (object);
  }
  catch (...)
  {
    LuaStack::Pop (1);
    throw;
  }
}

/*
    Удаление аргументов из стека
*/

void LuaStack::Pop (size_t arguments_count)
{
  size_t stack_size = static_cast<size_t> (lua_gettop (state));

  if (arguments_count > stack_size)
    arguments_count = stack_size;

  lua_pop (state, (int)arguments_count);
}
