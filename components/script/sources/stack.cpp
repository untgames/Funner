#include <script.h>

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

using namespace script::lua;

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

StackItem Stack::Get (int item_number) const
{
  if (item_number >= 0)
    return StackItem (state, item_number + 1);
  return StackItem (state, lua_gettop (state) + item_number + 1);
}

void Stack::Push (double arg)
{
  lua_pushnumber (state, arg);
}        

void Stack::Push (int arg)
{
  lua_pushinteger (state, arg);
}

void Stack::Push (const char* arg)
{
  lua_pushstring (state, arg);
}

void Stack::Push (void* arg)
{
  lua_pushlightuserdata (state, arg);
}

void Stack::PushFunction (const char* f_name)
{
  lua_getglobal(state, f_name);
}

void Stack::Pop (size_t count)
{
  lua_pop (state, (int)count);
}
