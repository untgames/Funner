#include <common/exception.h>
#include <stl/vector>
#include <stl/string>
#include <xtl/functional_fwd>
#include <stl/hash_map>
#include "shared.h"

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#define RECALL_NAME "recall"
#define HASH_MAP_NAME "funcs_hash_map"

using namespace common;
using namespace stl;
using namespace scriptlib;

enum ArgType
{
  Int,
  Float,
  Double,
  CharPtr,
  Void
};

union Variant
{
  int    Int;
  float  Float;
  double Double;
  const char* ConstCharPtr;
};

struct FuncSpec
{
  vector <ArgType> arg_types;
  ScriptEnv::MegaFunction fn;
};

class LuaImpl
{
  public:
    LuaImpl ();

    static int Recaller (lua_State* l_state);
    static int AtPanic  (lua_State* l_state) {Raise <Exception> ("LuaImpl::AtPanic", "Lua at panic."); return 0;}
    
    lua_State* l_state;
    hash_map <string, FuncSpec> funcs;
};

LuaImpl::LuaImpl ()
{
  l_state = lua_open ();
  if (!l_state)
    Raise <Exception> ("LuaImpl::LuaImpl", "Can't create lua state.");

  lua_atpanic (l_state, &LuaImpl::AtPanic);

  luaL_openlibs (l_state);

  lua_pushcfunction (l_state, &LuaImpl::Recaller);
  lua_setglobal     (l_state, RECALL_NAME);
  lua_pushlightuserdata (l_state, &funcs);
  lua_setglobal         (l_state, HASH_MAP_NAME);
}

int LuaImpl::Recaller (lua_State* l_state)
{
  lua_getglobal (l_state, HASH_MAP_NAME);
  hash_map<string, FuncSpec>::iterator func = ((hash_map <string, FuncSpec>*)lua_touserdata(l_state, -1))->find (string (lua_tostring (l_state, 1)));


  if (func == ((hash_map <string, FuncSpec>*)lua_touserdata(l_state, -1))->end ())
    Raise <Exception> ("LuaImpl::Recaller", "Calling unregistered function.");

  vector <Variant> arg_list (func->second.arg_types.size ());
  vector <string>  str_args;

  for (size_t i = 1; i < func->second.arg_types.size (); i++)
  {
    switch (func->second.arg_types[i])
    {
      case Int:     if (lua_isnumber (l_state, i+1))
                    {
                      arg_list[i-1].Int = lua_tointeger (l_state, i+1); 
                      break;
                    }
                    Raise <Exception> ("LuaImpl::Recaller", "Argument %u expected as integer, but it is not.", i);
      case Float:   if (lua_isnumber (l_state, i+1))
                    {
                      arg_list[i-1].Float = (float) lua_tonumber (l_state, i+1); 
                      break;
                    }
                    Raise <Exception> ("LuaImpl::Recaller", "Argument %u expected as float, but it is not.", i);   
      case Double:  if (lua_isnumber (l_state, i+1))
                    {
                      arg_list[i-1].Double = lua_tonumber (l_state, i+1); 
                      break;
                    }
                    Raise <Exception> ("LuaImpl::Recaller", "Argument %u expected as double, but it is not.", i);
      case CharPtr: if (lua_isstring (l_state, i+1))
                    {
                      str_args.push_back (lua_tostring (l_state, i+1));
                      arg_list[i-1].ConstCharPtr = str_args.back().c_str();
                      break;
                    }
                    Raise <Exception> ("LuaImpl::Recaller", "Argument %u expected as string, but it is not.", i);
      default:      Raise <Exception> ("LuaImpl::Recaller", "Unknown argument %u type", i);
    }
  }

  Variant ret_value;
  func->second.fn (arg_list[0].ConstCharPtr);

  if (func->second.arg_types[0] != Void)
  {
    switch (func->second.arg_types[0])
    {
      case Int:     lua_pushinteger (l_state, ret_value.Int); break;
      case Float:   lua_pushnumber (l_state, ret_value.Float); break;
      case Double:  lua_pushnumber (l_state, ret_value.Double); break;
      case CharPtr: lua_pushstring (l_state, ret_value.ConstCharPtr); break;
    }
    return 1;
  }
  return 0;
}

LuaEnvImpl::LuaEnvImpl ()
  : impl (new LuaImpl)
{
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

void LuaEnvImpl::BindFunction (const char* name, ScriptEnv::MegaFunction fn, size_t arg_count)
{
  string generated_function ("\nfunction ");
  string args;
 
  generated_function += name;
  generated_function += " (";
  for (size_t i = 0; i < arg_count; i++)
  {
    char arg_buf [8];

    sprintf (&arg_buf[0], "arg%u", i);
    args += &arg_buf[0];
    if (i < arg_count - 1)
      args += ", ";
  }
  generated_function += args;
  generated_function += ")\n return ";
  generated_function += RECALL_NAME;
  generated_function += " (\"";
  generated_function += name;
  generated_function += "\"";
  if (arg_count)
  {
    generated_function += ", ";
    generated_function += args;
  }
  generated_function += ")\n end";

  if (luaL_dostring (impl->l_state, generated_function.c_str()))
    Raise <Exception> ("LuaEnvImpl::BindFunction", "Error when registering function %s", name);

  FuncSpec spec;

  spec.arg_types.resize (arg_count + 1);
  spec.arg_types[0] = Void;
  spec.arg_types[1] = CharPtr;
  spec.fn = fn;

  impl->funcs.insert_pair (name, spec);
}

