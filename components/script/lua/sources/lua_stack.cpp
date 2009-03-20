#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;

namespace
{

/*
    Константы
*/

const char INLINE_FUNCTION_MARKER = '@'; //маркер встраиваемых команд

}

/*
    Конструкторы
*/

Stack::Stack (lua_State* in_state, lua::Interpreter& in_interpreter)
  : state (in_state),
    interpreter (in_interpreter)
  {}
  
/*
    Интерпретатор, которому принадлежит стек
*/

IInterpreter& Stack::Interpreter ()
{
  return interpreter;
}

/*
    Количество аргументов в стеке
*/

size_t Stack::Size ()
{
  return lua_gettop (state) + 1;
}

/*
    Получение аргументов из стека
*/

float Stack::GetFloat (size_t index)
{
  check_item (state, index, LUA_TNUMBER, "script::lua::Stack::GetFloat");
  
  return (float)lua_tonumber (state, index);
}

int Stack::GetInteger (size_t index)
{
  check_item (state, index, LUA_TNUMBER, "script::lua::Stack::GetInteger");

  return lua_tointeger (state, index);
}

bool Stack::GetBoolean (size_t index)
{
  check_item (state, index, LUA_TBOOLEAN, "script::lua::Stack::GetBoolean");
  
  return lua_toboolean (state, index) != 0;
}

void* Stack::GetPointer (size_t index)
{
  check_item (state, index, LUA_TLIGHTUSERDATA, "script::lua::Stack::GetPointer");
  return lua_touserdata (state, index);
}

const char* Stack::GetString (size_t index)
{
  check_item (state, index, LUA_TSTRING, "script::lua::Stack::GetString");  
  return lua_tostring (state, index);  
}

ISymbol* Stack::GetSymbol (size_t index)
{
  try
  {
      //проверка индекса

    check_item (state, index, LUA_TFUNCTION, "");
    
    int item_type = lua_type (state, index);
    
    switch (item_type)
    {
      case LUA_TFUNCTION:
      {
          //получение символа      

        return interpreter.SymbolRegistry ().GetSymbol (-(int)index);
      }
      case LUA_TSTRING:
      {
        check_stack (state, 1);
        
        const char* symbol_string = lua_tostring (state, index);
        
        if (!symbol_string)
          throw xtl::format_exception<RuntimeException> ("", "Null symbol string");

        if (*symbol_string == INLINE_FUNCTION_MARKER)
        {
          ++symbol_string; //удаление маркера
          
          if (luaL_loadbuffer (state, symbol_string, strlen (symbol_string), symbol_string))
            raise_error (state, "");
        }
        else
        {
          lua_getglobal (state, symbol_string);
        }

        ISymbol* symbol = 0;

        try
        {
          symbol = interpreter.SymbolRegistry ().GetSymbol (-1);
        }
        catch (...)
        {
          lua_pop (state, 1);
          throw;          
        }        

        lua_pop (state, 1);

        return symbol;
      }
      default:
        throw xtl::format_exception<StackException> ("", "Wrong item type");
    }    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::lua::Stack::GetSymbol");
    throw;
  }
}

xtl::any& Stack::GetVariant (size_t index)
{
    //проверка индекса
    
  check_item (state, index, LUA_TUSERDATA, "script::lua::Stack::GetVariant");

    //получение аргумента

  xtl::any* variant = reinterpret_cast<xtl::any*> (lua_touserdata (state, index));
  
  if (variant && lua_getmetatable (state, index))
  {
      //все пользовательские типы данных, хранимые в стеке, приводятся к xtl::any*. проверка совпадения метатаблиц не требуется
    
    lua_pop (state, 1);
    return *variant;
  }

    //если типы не совпали

  luaL_typerror (state, index, "xtl::any");

    //генерация исключения

  throw xtl::format_exception<script::ArgumentException> ("script::Stack::GetVariant", "Item %u has wrong type (non xtl::any)", index);
  
  return *variant;
}

/*
    Помещение аргументов в стек
*/

void Stack::Push (float value)
{
  check_stack    (state);
  lua_pushnumber (state, value);
}        

void Stack::Push (int value)
{
  check_stack     (state);  
  lua_pushinteger (state, value);
}

void Stack::Push (bool value)
{
  check_stack     (state);
  lua_pushboolean (state, value);
}

void Stack::Push (const char* string)
{
  if (!string)
    throw xtl::make_null_argument_exception ("script::lua::Stack::Push", "string");

  check_stack    (state);
  lua_pushstring (state, string);
}

void Stack::Push (void* value)
{
  check_stack (state);
  
  if (value) lua_pushlightuserdata (state, value);
  else       lua_pushnil           (state);
}

void Stack::PushSymbol (const char* symbol)
{
  if (!symbol)
    throw xtl::make_null_argument_exception ("script::lua::Stack::PushSymbol(const char*)", "symbol");

  check_stack   (state);    
  lua_getglobal (state, symbol);
}

void Stack::PushSymbol (ISymbol* symbol)
{
  if (!symbol)
    throw xtl::make_null_argument_exception ("script::lua::Stack::PushSymbol(ISymbol*)", "symbol");

  check_stack   (state);
  lua_getglobal (state, symbol->Name ());
}

void Stack::Push (const xtl::any& value)
{
  check_stack (state);    
  
  if (value.null ())
  {
      //обработка специального случая: помещение в стек нулевого указателя

    lua_pushnil (state);

    return;
  }

  static const size_t BUFFER_SIZE = sizeof (xtl::any); 

  void* buffer = lua_newuserdata (state, BUFFER_SIZE); 

  if (!buffer) 
    throw xtl::format_exception<StackException> ("script::lua::Stack::Push(const xtl::any&)", "Fail to allocate %u bytes from stack", BUFFER_SIZE);
    
  const char* library_id = interpreter.Environment ().FindLibraryId (value.castable_type ());
  
  if (!library_id)
    library_id = VARIANT_DEFAULT_TYPE_NAME;    

  luaL_getmetatable (state, library_id);
  lua_setmetatable  (state, -2);

  try
  {
    new (buffer) xtl::any (value);
  }
  catch (...)
  {
    lua_pop (state, 1);
    throw; 
  }
}

/*
    Удаление аргументов из стека
*/

void Stack::Pop (size_t arguments_count)
{
  size_t stack_size = static_cast<size_t> (lua_gettop (state));

  if (arguments_count > stack_size)
    arguments_count = stack_size;

  lua_pop (state, (int)arguments_count);
}
