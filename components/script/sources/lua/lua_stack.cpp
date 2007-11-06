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

LuaStack::LuaStack (lua_State* in_state)
  : state (in_state)
  {}

/*
    Количество аргументов в стеке
*/

size_t LuaStack::Size ()
{
  return lua_gettop (state);
}

/*
    Получение аргументов из стека
*/

namespace
{

//проверка наличия элемента в стеке
int get_item_index (lua_State* state, size_t index, const char* function_name)
{
  size_t stack_size = lua_gettop (state);

  if (index >= stack_size)
    Raise<StackException> (function_name, "Attempt to get argument %u from stack with %u items", index, stack_size);

  return index + 1;
}

}

float LuaStack::GetFloat (size_t index)
{
  return (float)lua_tonumber (state, get_item_index (state, index, "script::lua::LuaStack::GetFloat"));
}

int LuaStack::GetInteger (size_t index)
{
  return lua_tointeger (state, get_item_index (state, index, "script::lua::LuaStack::GetInteger"));
}

void* LuaStack::GetPointer (size_t index)
{
  return lua_touserdata (state, get_item_index (state, index, "script::lua::LuaStack::GetPointer"));
}

const char* LuaStack::GetString (size_t index)
{
  return lua_tostring (state, get_item_index (state, index, "script::lua::LuaStack::GetString"));
}

const char* LuaStack::GetSymbol (size_t index)
{
  RaiseNotSupported ("script::lua::LuaStack::GetSymbol");
  
  return "";
}

xtl::any& LuaStack::GetVariant (size_t index)
{
  return *reinterpret_cast<xtl::any*> (luaL_checkudata (state, get_item_index (state, index, "script::lua::LuaStack::GetVariant"), USER_DATA_TAG));
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
    Raise<StackException> ("script::lua::LuStack::Push(const xtl::any&)", "Fail to allocate %u bytes from lua stack", BUFFER_SIZE);

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
