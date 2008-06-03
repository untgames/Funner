#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;
using namespace xtl;

/*
    Имя вариантного типа данных по умолчанию
*/

namespace script
{

namespace lua
{

const char* VARIANT_DEFAULT_TYPE_NAME = "__lua_variant_type";

}

}

namespace
{

//функция обработки ошибок lua
int error_handler (lua_State* state)
{
  raise<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));

  return 0;
}

}

/*
    Конструктор / деструктор
*/

Interpreter::Interpreter (const EnvironmentPointer& in_environment)
  : environment (in_environment),
    stack (state, *environment)
{
    //инициализация стандартных библиотек

  luaopen_base   (state);
  luaopen_string (state);
  luaopen_table  (state);    

    //регистрация функции обработки ошибок

  lua_atpanic (state, &error_handler);
  
    //регистрация обработчиков пользовательского типа данных

  static const luaL_reg user_data_meta_table [] = {
    {"__gc",       &variant_destroy},
    {"__tostring", &variant_tostring},
    {0,0}
  };
  
  luaL_newmetatable (state, VARIANT_DEFAULT_TYPE_NAME);
  luaI_openlib      (state, 0, user_data_meta_table, 0);

    //регистрация обработчиков событий создания/удаления библиотек

  on_create_library_connection = environment->RegisterEventHandler (EnvironmentLibraryEvent_OnCreate,
    xtl::bind (&Interpreter::RegisterLibrary, this, _2, _3));

  on_remove_library_connection = environment->RegisterEventHandler (EnvironmentLibraryEvent_OnRemove,
    xtl::bind (&Interpreter::UnregisterLibrary, this, _2));

    //регистрация библиотек

  for (Environment::Iterator i=environment->CreateIterator (); i; ++i)
    RegisterLibrary (environment->LibraryId (i), *i);

    //очистка стека

  lua_settop (state, 0);
}

Interpreter::~Interpreter ()
{
}

/*
    Имя интерпретатора
*/

const char* Interpreter::Name ()
{
  return LUA_RELEASE;
}

/*
    Стек аргументов
*/

IStack& Interpreter::Stack ()
{
  return stack;
}

/*
    Проверка наличия функции
*/

bool Interpreter::HasFunction (const char* name)
{
  if (!name)
    return false;
      
  lua_getglobal (state, name);

  bool is_function = lua_isfunction (state, -1) != 0;

  lua_pop (state, 1);

  return is_function;
}

/*
    Выполнение буфера интерпретации луа
*/

namespace
{

//функция протоколирования по умолчанию
void log_function (const char*)
{
}

}

void Interpreter::DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log)
{
    //загружаем буфер команд в контекст луа

  if (luaL_loadbuffer (state, (const char*)buffer, buffer_size, buffer_name))
  {
    try
    {
      log (lua_tostring (state, -1));        
    }
    catch (...)
    {
      //подавляем все исключения
    }

      //выталкиваем сообщение об ошибке из стека 

    lua_pop (state, 1);

    return;
  }

  if (lua_pcall (state, 0, LUA_MULTRET, 0))
  {
    try
    {
      log (lua_tostring (state, -1));
    }
    catch (...)
    {
      //подавляем все исключения
    }
    
      //выталкиваем из стека сообщение об ошибке
      
    lua_pop (state, 1);
  }
}

/*
    Вызов функции луа
*/

void Interpreter::Invoke (size_t arguments_count, size_t results_count)
{
    //возможно проще использовать call???

  if (lua_pcall (state, arguments_count, results_count, 0))
  {
      //формируем сообщение об ошибке
    
    const char* reason    = lua_tostring (state, -1);
    stl::string error_msg = reason ? reason : "internal error";

      //выталкиваем из стека сообщение об ошибке

    lua_pop (state, 1);

      //возбуждаем исключение

    raise<RuntimeException> ("script::lua::Interpreter::Invoke", "%s", error_msg.c_str ());    
  }
}

/*
    Подсчёт ссылок
*/

void Interpreter::AddRef ()
{
  addref (this);
}

void Interpreter::Release ()
{
  release (this);
}
   
/*
    Регистрация / удаление библиотек
*/

void Interpreter::RegisterLibrary (const char* name, InvokerRegistry& registry)
{
  if (!name)
    raise_null_argument ("script::lua::RegisterLibrary", "name");

  libraries.insert_pair (name, LibraryPtr (new Library (*this, name, registry), false));
}

void Interpreter::UnregisterLibrary (const char* name)
{
  if (!name)
    return;
    
  libraries.erase (name);
}

namespace script
{

/*
    Создание интерпретатора lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const xtl::shared_ptr<Environment>& environment)
{
  if (!environment)
    common::raise_null_argument ("script::create_lua_interpreter", "environment");

  return xtl::com_ptr<IInterpreter> (new Interpreter (environment), false);
}

}
