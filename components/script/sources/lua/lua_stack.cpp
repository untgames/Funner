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

Stack::Stack ()
  : state (0)
  {}

Stack::Stack (lua_State* in_state)
  : state (in_state)
  {}  
  
/*
    Состояние машины Lua
*/

void Stack::SetState (lua_State* in_state)
{
  state = in_state;
}

/*
    Количество аргументов в стеке
*/

size_t Stack::Size ()
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

float Stack::GetFloat (size_t index)
{
  check_item_index (state, index, "script::lua::Stack::GetFloat");
  return (float)lua_tonumber (state, index);
}

int Stack::GetInteger (size_t index)
{
  check_item_index (state, index, "script::lua::Stack::GetInteger");
  return lua_tointeger (state, index);
}

void* Stack::GetPointer (size_t index)
{
  check_item_index (state, index, "script::lua::Stack::GetPointer");
  return lua_touserdata (state, index);
}

const char* Stack::GetString (size_t index)
{
  check_item_index (state, index, "script::lua::Stack::GetString");
  return lua_tostring (state, index);
}

const char* Stack::GetSymbol (size_t index)
{
  RaiseNotSupported ("script::lua::Stack::GetSymbol");
  
  return "";
}

xtl::any& Stack::GetVariant (size_t index)
{
    //проверка индекса
    
  check_item_index (state, index, "script::lua::Stack::GetVariant");

    //получение аргумента

  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, index));
  
  return *variant; //for tests only!!!

    //получение мета-таблицы

/*  if (variant && lua_getmetatable (state, index))
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

  Raise<RuntimeException> ("script::Stack::GetVariant", "Item %u has wrong type (non user-data)", index);
  
  return *variant;*/
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
    Raise<StackException> ("script::lua::Stack::Push", "Not enough stack space."
    "Attempt to push %u items in stack with %u items (stack_size=%u)", count, lua_gettop (state), LUAI_MAXCSTACK);
}

}

void Stack::Push (float value)
{
  check_stack    (state);
  lua_pushnumber (state, value);
}        

void Stack::Push (int value)
{
  check_stack    (state);
  lua_pushinteger (state, value);
}

void Stack::Push (const char* string)
{
  if (!string)
    RaiseNullArgument ("script::lua::Stack::Push", "string");

  check_stack    (state);
  lua_pushstring (state, string);
}

void Stack::Push (void* value)
{
  check_stack           (state);
  lua_pushlightuserdata (state, value);
}

void Stack::PushSymbol (const char* symbol)
{
  if (!symbol)
    RaiseNullArgument ("script::lua::Stack::PushSymbol", "symbol");

  check_stack   (state);    
  lua_getglobal (state, symbol);
}

void Stack::Push (const xtl::any& value)
{
//  static const size_t BUFFER_SIZE = sizeof (xtl::any);  
//  void* buffer = lua_newuserdata (state, BUFFER_SIZE);
  xtl::any* object = new xtl::any (value);

//  if (!buffer)
//    Raise<StackException> ("script::lua::Stack::Push(const xtl::any&)", "Fail to allocate %u bytes from lua stack", BUFFER_SIZE);

  lua_pushlightuserdata (state, object);
//  luaL_getmetatable (state, USER_DATA_TAG);
  luaL_getmetatable (state, object->type ().name ()); //for tests only!!!
  lua_setmetatable  (state, -2);
  
  printf ("push '%s'\n", object->type ().name ());

/*  try
  {
    new (buffer) xtl::any (object);
  }
  catch (...)
  {
    Stack::Pop (1);
    throw;
  }*/
}

/*
    Удаление аргументов из стека
*/

void Stack::Pop (size_t arguments_count)
{
  size_t stack_size = static_cast<size_t> (lua_gettop (state));

  if (arguments_count > stack_size)
    arguments_count = stack_size;

  lua_pop (state, (int)arguments_count);
}
