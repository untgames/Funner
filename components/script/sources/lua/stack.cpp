#include "shared.h"

using namespace script::lua::detail;
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
    Конструктор
*/

Stack::Stack (lua_State* in_state)
  : state (in_state)
{
  if (!state)
    RaiseNullArgument ("script::lua::Stack::Stack", "state");
}
  
/*
    Помещение элементов базовых типов в стек
*/

namespace
{

//проверка возможности поместить в стек count аргументов 
void check_stack (lua_State* state, size_t count = 1)
{
  if (!lua_checkstack (state, count))
    Raise<StackOverflowException> ("script::lua::Stack::Push", "Not enough stack space."
    "Attempt to push %u items in stack with %u items (stack_size=%u)", count, lua_gettop (state), LUAI_MAXCSTACK);
}

}

void Stack::Push (double arg)
{
  check_stack    (state);
  lua_pushnumber (state, arg);
}        

void Stack::Push (int arg)
{
  check_stack    (state);
  lua_pushinteger (state, arg);
}

void Stack::Push (const char* string)
{
  if (!string)
    RaiseNullArgument ("script::lua::Stack::Push", "string");

  check_stack    (state);
  lua_pushstring (state, string);
}

void Stack::Push (void* arg)
{
  check_stack           (state);
  lua_pushlightuserdata (state, arg);
}

void Stack::PushFunction (const char* function_name)
{
  if (!function_name)
    RaiseNullArgument ("script::lua::Stack::PushFunction", "function_name");

  check_stack   (state);    
  lua_getglobal (state, function_name);
}

/*
    Получение из стека элементов базовых типов
*/

namespace
{

//проверка наличия элемента в стеке
int get_item_index (lua_State* state, int index)
{
   //??????????
   //сделать проверку корректности индекса (сверху и снизу)
   
  return index >= 0 ? index + 1 : lua_gettop (state) + index + 1;
}

}

double Stack::GetDouble (int index) const
{
  return lua_tonumber (state, get_item_index (state, index));
}

int Stack::GetInteger (int index) const
{
  return lua_tointeger (state, get_item_index (state, index));
}

void* Stack::GetPointer (int index) const
{
  return lua_touserdata (state, get_item_index (state, index));
}

const char* Stack::GetString (int index) const
{
  return lua_tostring (state, get_item_index (state, index));
}

/*
    Помещение/извлечение из стека пользовательского типа данных
*/

void* Stack::NewUserData (size_t size)
{
  void* buffer = lua_newuserdata (state, size);
  
  if (!buffer)
    Raise<StackOverflowException> ("script::lua::Stack::NewUserData", "Failt allocation of %u bytes from lua stack", size);

  luaL_getmetatable (state, USER_DATA_TAG);
  lua_setmetatable  (state, -2);

  return buffer;
}

IUserData* Stack::GetUserData (int index) const
{
  return reinterpret_cast<IUserData*> (luaL_checkudata (state, get_item_index (state, index), USER_DATA_TAG));
}

/*
    Удаление из стека count элементов
*/

void Stack::Pop (size_t count)
{
  size_t stack_size = static_cast<size_t> (lua_gettop (state));

  if (count > stack_size)
    count = stack_size;

  lua_pop (state, (int)count);
}
