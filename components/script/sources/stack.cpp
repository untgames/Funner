#include <script.h>
#include <common/exception.h>

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

using namespace script::lua::detail;
using namespace common;

StackItem::StackItem (lua_State* in_state, size_t index)
  : state (in_state), argument_number (index)
{
}

StackItem::operator float () const
{
  return (float)lua_tonumber (state, argument_number);
}

StackItem::operator double () const
{
  return lua_tonumber (state, argument_number);
}

StackItem::operator int () const
{
  return lua_tointeger (state, argument_number);
}

StackItem::operator size_t () const
{
  return lua_tointeger (state, argument_number);
}

StackItem::operator const char* () const
{
  return lua_tostring (state, argument_number);
}

StackItem::operator const void* () const
{
  return lua_touserdata (state, argument_number);
}

Stack::Stack (lua_State* in_state)
  : state (in_state)
{
}

size_t Stack::Size () const
{
  return lua_gettop (state);
}

int Stack::CheckAvailable (size_t count) const
{
  return lua_checkstack (state, count);
}

StackItem Stack::Get (int item_number) const
{
  if (item_number >= 0)
    return StackItem (state, item_number + 1);
  return StackItem (state, lua_gettop (state) + item_number + 1);
}

void Stack::Push (double arg)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_pushnumber (state, arg);
}        

void Stack::Push (float arg)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_pushnumber (state, arg);
}        

void Stack::Push (int arg)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_pushinteger (state, arg);
}

void Stack::Push (size_t arg)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_pushinteger (state, arg);
}

void Stack::Push (const char* arg)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_pushstring (state, arg);
}

void Stack::Push (void* arg)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_pushlightuserdata (state, arg);
}

void Stack::PushFunction (const char* f_name)
{
  if (!CheckAvailable (1))
    Raise <Exception> ("Stack::Push", "Not enough stack space");
  lua_getglobal(state, f_name);
}

void* Stack::Alloc (size_t size)
{
  void *ret_value = lua_newuserdata (state, size);
  luaL_getmetatable(state, "iuser_data");
  lua_setmetatable(state, -2);
  return ret_value; 
}

void Stack::Pop (size_t count)
{
  if (count > Size ())
    lua_pop (state, (int)Size ());
  else
    lua_pop (state, (int)count);    
}
