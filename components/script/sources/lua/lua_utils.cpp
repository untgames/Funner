#include "shared.h"

using namespace common;
using namespace script::lua;

namespace script
{

namespace
{

//получение строки с местом вызова
stl::string get_lua_position (lua_State* state)
{
  stl::string buffer;  
  lua_Debug   debug;
  
  for (int level=0; ; level++)
  {
    if (!lua_getstack (state, level, &debug))
      break;
      
    lua_getinfo (state, "Sln", &debug);
   
    buffer += common::format ("\n    at lua-call %s(%d): function='%s'", debug.short_src, debug.currentline, debug.namewhat);
  }

  return buffer;
}

//безопасный вызов шлюзов
template <class Fn> int safe_call (lua_State* state, Fn f)
{
  try
  {
    return f (state);
  }
  catch (xtl::bad_any_cast& exception)
  {
    luaL_error (state, "%s: %s -> %s%s", exception.what (), exception.source_type ().name (), exception.target_type ().name (),
                get_lua_position (state).c_str ());
  }
  catch (std::exception& exception)
  {
    luaL_error (state, "exception: %s%s", exception.what (), get_lua_position (state).c_str ());
  }
  catch (...)
  {
    luaL_error (state, "%s%s", lua_gettop (state) ? lua_tostring (state, -1) : "internal error", get_lua_position (state).c_str ());
  }

  return 0;
}

//диспетчер вызовов
int unsafe_invoke_dispatch (lua_State* state)
{
    //получение указателя на шлюз      

  const Invoker* invoker   = reinterpret_cast<const Invoker*> (lua_touserdata (state, lua_upvalueindex (1)));
  Interpreter* interpreter = reinterpret_cast<Interpreter*> (lua_touserdata (state, lua_upvalueindex (2)));

  if (!invoker || !interpreter)
    Raise<RuntimeException> ("script::lua::invoke_dispatch", "Bad invoker call (no up-values found)");

    //проверка количества переданных аргументов

  if ((int)invoker->ArgumentsCount () > lua_gettop (state))
    Raise<StackException> ("script::lua::invoke_dispatch", "Arguments count mismatch (expected %u, got %u)", 
                           invoker->ArgumentsCount (), lua_gettop (state));

    //вызов шлюза

  (*invoker)(interpreter->Interpreter::Stack ());

  return invoker->ResultsCount ();
}

//функция, вызываемая сборщиком мусора при удалении объектов пользовательского типа данных
int unsafe_destroy_object (lua_State* state)
{
  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, -1));
  
  if (variant && lua_getmetatable (state, -1))
  {
      //все пользовательские типы данных, хранимые в стеке, приводятся к xtl::any*. проверка совпадения метатаблиц не требуется
    
    lua_pop (state, 1);

    delete variant;
  }

  return 0;
}

//печать в строку состояния объекта пользовательского типа данных
int unsafe_dump_to_string (lua_State* state)
{
  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, -1));
  
  if (variant && lua_getmetatable (state, -1))
  {
    lua_pop (state, 1);
    
    stl::string buffer;

    to_string      (buffer, *variant);
    lua_pushstring (state, buffer.c_str ());

    return 1;
  }

  return 0;
}

}

namespace lua
{

//вызов шлюза
int invoke_dispatch (lua_State* state)
{
  return safe_call (state, &unsafe_invoke_dispatch);
}

//удаление объекта
int destroy_object (lua_State* state)
{
  return safe_call (state, &unsafe_destroy_object);
}

//функция обработки ошибок lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));

  return 0;
}

//печать в строку состояния объекта пользовательского типа данных
int dump_to_string (lua_State* state)
{
  return safe_call (state, &unsafe_dump_to_string);
}

//функция заказа памяти
void* reallocate (void* user_data, void* ptr, size_t old_size, size_t new_size)
{
  try
  {
    common::Heap& heap = *reinterpret_cast<common::Heap*> (user_data);

    if (!new_size)
    {
      heap.Deallocate (ptr);
      return 0;    
    }

    void* new_buffer = heap.Allocate (new_size);

    if (!new_buffer)
      return 0;    

    if (ptr)
    {
      memcpy (new_buffer, ptr, old_size < new_size ? old_size : new_size);

      heap.Deallocate (ptr);
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

}
