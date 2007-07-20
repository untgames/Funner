#include <common/exception.h>
#include "shared.h"

using namespace common;
using namespace scriptlib;

ScriptEnv::ScriptEnv (ScriptType script_type)
{
  switch (script_type)
  {
    case ST_LUA: impl = new LuaEnvImpl; break;
    default:     Raise <Exception> ("ScriptEnv::ScriptEnv", "Unknow script type received");
  }
}

ScriptEnv::ScriptEnv (ScriptEnvImpl* source)
  : impl (source)
{
}

ScriptEnv::~ScriptEnv ()
{
}

void ScriptEnv::Load (const char* file_name)
{  
  impl->Load (file_name);
}

const char* ScriptEnv::Name () const
{
  return impl->str_name.c_str ();
}

void ScriptEnv::Rename (const char* new_name)
{
  impl->str_name = new_name;
}

template <class T>
void ScriptEnv::SetVariable (const char* name, T value)
{
  impl->SetVariable (name, value);
}

template <class T>
T ScriptEnv::GetVariable (const char* name)
{
  return impl->GetVariable (name);
}

template<> const char* ScriptEnv::GetVariable <const char*> (const char* name)
{
  return impl->GetVariable (name);
}

/*const char* ScriptEnv::GetVariable (const char* name)
{
  return impl->GetVariable (name);
} */

void ScriptEnv::Swap (ScriptEnv& script)
{
  swap (impl, script.impl);
}

namespace scriptlib
{

void swap (ScriptEnv& env1,ScriptEnv& env2)
{
  env1.Swap (env2);
}

}
