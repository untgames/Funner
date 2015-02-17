#include "shared.h"

using namespace common;
using namespace script::lua;

namespace
{

/*
    Константы
*/

const size_t STACK_DUMP_BUFFER_RESERVE = 128; //размер строки, резервируемый для дампа стека луа

/*
    Функции
*/

//функция, вызываемая сборщиком мусора при удалении объектов пользовательского типа данных
struct VariantDestroy 
{
  static int Invoke (lua_State* state)
  {
    xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, -1));

    if (variant && lua_getmetatable (state, -1))
    {
        //все пользовательские типы данных, хранимые в стеке, приводятся к xtl::any*. проверка совпадения метатаблиц не требуется
      
      lua_pop (state, 1);
      
      stl::destroy (variant);
    }

    return 0;
  }
};

//печать в строку состояния объекта пользовательского типа данных
struct VariantToString
{
  static int Invoke (lua_State* state)
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
};

}

namespace script
{

namespace lua
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
    
    if (debug.currentline == -1)
      continue;
    
    if (debug.name) buffer += common::format ("\n    at %s(%d): function=\"%s\"", debug.short_src, debug.currentline, debug.name);
    else            buffer += common::format ("\n    at %s(%d)", debug.short_src, debug.currentline);
  }

  return buffer;
}

//удаление объекта
int variant_destroy (lua_State* state)
{
  return safe_call<VariantDestroy> (state);
}

//печать в строку состояния объекта пользовательского типа данных
int variant_tostring (lua_State* state)
{
  return safe_call<VariantToString> (state);
}

//печать состояния стека
void dump_stack (lua_State* state, stl::string& buffer)
{
    //резервирование памяти для буфера дампа стека
    
  buffer.clear   ();
  buffer.reserve (STACK_DUMP_BUFFER_RESERVE);
  
    //перебор элементов стека

  int count = lua_gettop (state) + 1;    

  for (int i=1; i<count; i++)
  {
      //добавление разделителя
    
    if (!buffer.empty ())
      buffer += ", ";
      
      //печать элемента
      
    int type = lua_type (state, i);
      
    switch (type)
    {
      case LUA_TNIL:
        buffer += "NIL";
        break;
      case LUA_TBOOLEAN:
        buffer += lua_toboolean (state, i) ? "true" : "false";
        break;
      case LUA_TLIGHTUSERDATA:
        buffer += "<POINTER>";
        break;
      case LUA_TNUMBER:
        buffer += format ("%.4f", lua_tonumber (state, i));
        break;
      case LUA_TSTRING:
        buffer += format ("'%s'", lua_tostring (state, i));
        break;
      case LUA_TTABLE:
        buffer += "<TABLE>";
        break;
      case LUA_TFUNCTION:
        buffer += "<FUNCTION>";
        break;
      case LUA_TUSERDATA:
      {
        xtl::any* volatile variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, i));
        
        if (!variant)
        {
          buffer += "<NIL>";
          continue;
        }
        
        stl::string value;
        
        try
        {
          variant->to_string (value);
          
          buffer += format ("[%s]", value.c_str ());          
        }
        catch (...)
        {
          buffer += format ("<%s>", variant->castable_type ().name ());
        }

        break;
      }
      case LUA_TTHREAD:
        buffer += "<THREAD>";
        break;
      default:
        buffer += format ("<UNKNOWN: %s>", lua_typename (state, i));
        break;
    }
  }
}

//проверка возможности поместить в стек count аргументов 
void check_stack (lua_State* state, unsigned int count)
{
  if (!lua_checkstack (state, count))
    throw xtl::format_exception<StackException> ("script::lua::check_stack", "Not enough stack space."
    "Attempt to push %u items in stack with %u items (stack_size=%u)", count, lua_gettop (state), LUAI_MAXCSTACK);
}

namespace
{

//проверка наличия элемента в стеке
void check_item_index (lua_State* state, unsigned int index, const char* function_name)
{
  unsigned int stack_size = lua_gettop (state) + 1;

  if (index >= stack_size)
    throw xtl::format_exception<script::ArgumentException> (function_name, "Attempt to get item #%u from stack with %u items", index, stack_size);
}

}

//проверка корректности типа элемента, извлекаемого из стека
void check_item (lua_State* state, unsigned int index, int expected_type, const char* function_name)
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
        case LUA_TBOOLEAN:
          return;
      }
      break;
    case LUA_TNUMBER:
      switch (expected_type)
      {
        case LUA_TNUMBER:
        case LUA_TNIL:
        case LUA_TBOOLEAN:
          return;
      }
      
      break;
    case LUA_TBOOLEAN:
      switch (expected_type)
      {
        case LUA_TNUMBER:
        case LUA_TNIL:
        case LUA_TBOOLEAN:
          return;
      }
      break;
    case LUA_TSTRING:
      switch (expected_type)
      {
        case LUA_TSTRING:
        case LUA_TFUNCTION:
          return;
      }
      break;
    case LUA_TFUNCTION:
      switch (expected_type)
      {
        case LUA_TFUNCTION:
          return;
      }
      break;
    default:
      break;
  }
  
  if (item_type != expected_type)
  {
    throw xtl::format_exception<script::ArgumentException> (function_name, "Bad item #%u type (%s expected, got %s)", index, lua_typename (state, expected_type),
                             lua_typename (state, item_type));
  }  
}

//получение сообщения об ошибке
void raise_error (lua_State* state, const char* source)
{
  try
  {
    const char* reason = lua_tostring (state, -1);

    throw xtl::format_exception<RuntimeException> (source, "%s", reason ? reason : "internal error");
  }
  catch (...)
  {
    lua_pop (state, 1);
    throw;
  }
}

}

}
