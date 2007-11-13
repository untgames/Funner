#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

/*
    Конструктор / деструктор
*/

StateHolder::StateHolder ()
{
  state = lua_newstate (&reallocate, &MemoryManager::GetHeap ());

  if (!state)
    Raise <InterpreterException> ("script::lua::StateHolder::StateHolder", "Can't create lua state");
}

StateHolder::~StateHolder ()
{
  lua_close (state);
}
