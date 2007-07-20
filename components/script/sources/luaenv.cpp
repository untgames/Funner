#include <common/exception.h>
#include "shared.h"

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

using namespace common;
using namespace scriptlib;

class LuaImpl
{
  public:
    lua_State* l_state;
};

LuaEnvImpl::LuaEnvImpl ()
  : impl (new LuaImpl)
{
  impl->l_state = lua_open ();
  if (!impl->l_state)
    Raise <Exception> ("LuaEnvImpl::LuaEnvImpl", "Can't create lua state.");

  luaL_openlibs (impl->l_state);
}

LuaEnvImpl::~LuaEnvImpl ()
{
  lua_close (impl->l_state);
}

void LuaEnvImpl::Load (const char* file_name)
{  
  if (luaL_dofile (impl->l_state, file_name))
    Raise <Exception> ("LuaEnvImpl::Load", "Error when loading file %s", file_name);
}

void LuaEnvImpl::SetVariable (const char* name, const char* value)
{
}

const char* LuaEnvImpl::GetVariable (const char* name)
{
  lua_getglobal (impl->l_state, name);
  return lua_tostring (impl->l_state, -1);
}

