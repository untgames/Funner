#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

/*
    Конструкторы
*/

Stack::Stack (lua_State* in_state, Environment& in_environment)
  : state (in_state),
    environment (in_environment)
  {}

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
    Raise<script::ArgumentException> (function_name, "Attempt to get item #%u from stack with %u items", index, stack_size);
}

//проверка корректности типа элемента, извлекаемого из стека
void check_item (lua_State* state, size_t index, int expected_type, const char* function_name)
{
    //проверка корректности индекса элемента

  check_item_index (state, index, function_name);
  
    //определение типа элемента, находящегося в стеке

  int item_type = lua_type (state, index);
  
    //проверка совместимости типов
    
  switch (item_type)
  {
    case LUA_TNIL:
      switch (expected_type)
      {
        case LUA_TNUMBER:
        case LUA_TNIL:
        case LUA_TLIGHTUSERDATA:
          return;
      }
      break;
    case LUA_TBOOLEAN:
      switch (expected_type)
      {
        case LUA_TNUMBER:
        case LUA_TNIL:
          return;
      }
      break;
    default:
      break;
  }
  
  if (item_type != expected_type)
  {
    Raise<script::ArgumentException> (function_name, "Bad item #%u type (%s expected, got %s)", index, lua_typename (state, expected_type),
                             lua_typename (state, item_type));
  }  
}

}

float Stack::GetFloat (size_t index)
{
  check_item (state, index, LUA_TNUMBER, "script::lua::Stack::GetFloat");
  
  return (float)lua_tonumber (state, index);
}

int Stack::GetInteger (size_t index)
{
  check_item (state, index, LUA_TNUMBER, "script::lua::Stack::GetInteger");
  return lua_tointeger (state, index);
}

void* Stack::GetPointer (size_t index)
{
  check_item (state, index, LUA_TLIGHTUSERDATA, "script::lua::Stack::GetPointer");
  return lua_touserdata (state, index);
}

const char* Stack::GetString (size_t index)
{
  check_item (state, index, LUA_TSTRING, "script::lua::Stack::GetString");
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
    
  check_item (state, index, LUA_TUSERDATA, "script::lua::Stack::GetVariant");

    //получение аргумента

  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, index));
  
  if (variant && lua_getmetatable (state, index))
  {
      //все пользовательские типы данных, хранимые в стеке, приводятся к xtl::any*. проверка совпадения метатаблиц не требуется
    
    lua_pop (state, 1);
    return *variant;
  }

    //если типы не совпали

  luaL_typerror (state, index, "xtl::any");

    //генерация исключения

  Raise<script::ArgumentException> ("script::Stack::GetVariant", "Item %u has wrong type (non xtl::any)", index);
  
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
  check_stack (state);    
  
  if (value.null ())
  {
      //обработка специального случая: помещение в стек нулевого указателя

    lua_pushnil (state);

    return;
  }

  static const size_t BUFFER_SIZE = sizeof (xtl::any); 

  void* buffer = lua_newuserdata (state, BUFFER_SIZE); 

  if (!buffer) 
    Raise<StackException> ("script::lua::Stack::Push(const xtl::any&)", "Fail to allocate %u bytes from stack", BUFFER_SIZE);
    
  const char* library_id = environment.FindLibraryId (value.castable_type ());
  
  if (!library_id)
    library_id = VARIANT_DEFAULT_TYPE_NAME;    

  luaL_getmetatable (state, library_id);
  lua_setmetatable  (state, -2);

  try
  {
    new (buffer) xtl::any (value);
  }
  catch (...)
  {
    lua_pop (state, 1);
    throw; 
  }
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
