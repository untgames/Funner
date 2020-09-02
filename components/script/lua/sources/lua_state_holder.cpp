#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

/*
    Функция заказа памяти
*/

namespace
{

void* reallocate (void* user_data, void* ptr, size_t old_size, size_t new_size)
{
  try
  {
    if (!new_size)
    {
      MemoryManager::Deallocate (ptr);
      return 0;
    }

    void* new_buffer = MemoryManager::Allocate (new_size);

    if (!new_buffer)
      return 0;

    if (ptr)
    {
      memcpy (new_buffer, ptr, old_size < new_size ? old_size : new_size);

      MemoryManager::Deallocate (ptr);
    }

    return new_buffer;
  }
  catch (...)
  {
    //подавляем все исключения
    return 0;
  }
}

}

/*
    Конструктор / деструктор
*/

StateHolder::StateHolder ()
{
  state = lua_newstate (&reallocate, 0);

  if (!state)
    throw xtl::format_exception<InterpreterException> ("script::lua::StateHolder::StateHolder", "Can't create lua state");
}

StateHolder::~StateHolder ()
{
  lua_close (state);
}
