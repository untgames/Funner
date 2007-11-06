#include "shared.h"

using namespace script;
using namespace script::lua;

/*
    Конструктор / деструктор
*/

LuaInterpreter::LuaInterpreter (const InvokerRegistry& in_registry)
  : registry (in_registry), ref_count (1)
{
//  stack.SetState (environment.State ());
  
//  for (InvokerRegistry::Iterator i = ((InvokerRegistry)registry).CreateIterator (); i; ++i)
//    environment.RegisterFunction (registry.InvokerId (i), &LuaInterpreter::InvokeFunction);
}

LuaInterpreter::~LuaInterpreter ()
{
}

/*
    Имя интерпретатора
*/

const char* LuaInterpreter::Name ()
{
    //желательно добавить версию или строку из луа

  return "LuaInterpreter";
}

/*
    Стек аргументов
*/

IStack& LuaInterpreter::Stack ()
{
  return stack;
}

/*
    Проверка наличия функции
*/

bool LuaInterpreter::HasFunction (const char* name)
{
  return false; //!!!!!
}

/*
    Выполнение буфера интерпретации луа
*/

void LuaInterpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
}

/*
    Вызов функции луа
*/

void LuaInterpreter::Invoke (size_t arguments_count, size_t results_count)
{
}

/*
    Подсчёт ссылок
*/

void LuaInterpreter::AddRef ()
{
  ref_count++;
}

void LuaInterpreter::Release ()
{
  if (!--ref_count)
    delete this;
}

namespace script
{

/*
    Создание интерпретатора lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const InvokerRegistry& registry)
{
  return xtl::com_ptr<IInterpreter> (new LuaInterpreter (registry), false);
}

}
