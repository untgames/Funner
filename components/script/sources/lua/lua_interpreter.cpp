#include "shared.h"

using namespace script;
using namespace script::lua;
using namespace common;
using namespace xtl;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

//функция протоколирования по умолчанию
void log_function (const char*)
{
}

//функция обработки ошибок lua
int error_handler (lua_State* state)
{
  Raise<RuntimeException> ("script::lua::error_handler", "%s", lua_tostring (state, -1));

  return 0;
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

/*
    Конструктор / деструктор
*/

Interpreter::Interpreter (const EnvironmentPointer& in_environment)
  : environment (in_environment),
    ref_count (1)
{
    //инициализация состояния lua

  state = lua_newstate (&reallocate, &MemoryManager::GetHeap ());

  if (!state)
    Raise <InterpreterException> ("script::lua::Interpreter::Interpreter", "Can't create lua state");

  try
  {
    luaL_openlibs (state);

      //регистрация обработчиков удаления пользовательских типов данных    

    static const luaL_reg user_data_meta_table [] = {
      {"__gc", &destroy_object},
      {0,0}
    };
    
    luaL_newmetatable (state, USER_DATA_TAG);
    luaI_openlib      (state, 0, user_data_meta_table, 0); //must be 1

      //регистрация функции обработки ошибок

    lua_atpanic (state, &error_handler);

      //инициализация стека

    stack.SetState (state);
    
      //регистрация обработчиков событий создания/удаления реестров
      
    on_create_registry_connection = environment->RegisterEventHandler (EnvironmentEvent_OnCreateRegistry,
      xtl::bind (&Interpreter::RegisterInvokerRegistry, this, _2, _3));
      
    on_remove_registry_connection = environment->RegisterEventHandler (EnvironmentEvent_OnRemoveRegistry,
      xtl::bind (&Interpreter::UnregisterInvokerRegistry, this, _2));
      
      //регистрация обработчиков событий появления новых реестров

    for (Environment::Iterator i=environment->CreateIterator (); i; ++i)
      RegisterInvokerRegistry (environment->RegistryId (i), *i);

      //очистка стека

    lua_settop (state, 0);
  }
  catch (...)
  {
    lua_close (state);
    throw;
  }
}

Interpreter::~Interpreter ()
{
    //удаление метатаблиц
    
  for (MetatableMap::iterator i=metatables.begin (), end=metatables.end (); i!=end; ++i)
    delete i->second;
    
  metatables.clear ();
    
    //закрытие lua

  lua_close (state);
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
  if (lua_pcall (state, arguments_count, results_count, 0))
  {
      //формируем сообщение об ошибке
    
    const char* reason    = lua_tostring (state, -1);
    stl::string error_msg = reason ? reason : "internal error";

      //выталкиваем из стека сообщение об ошибке

    lua_pop (state, 1);

      //возбуждаем исключение

    Raise<RuntimeException> ("script::lua::Interpreter::Invoke", "%s", error_msg.c_str ());    
  }
}

/*
    Подсчёт ссылок
*/

void Interpreter::AddRef ()
{
  ref_count++;
}

void Interpreter::Release ()
{
  if (!--ref_count)
    delete this;
}

/*
   Регистрация / удаление шлюзов
*/

void Interpreter::RegisterInvoker (const char* invoker_name, Invoker& invoker)
{
  if (!invoker_name)
    RaiseNullArgument ("script::lua::Interpreter::RegisterInvoker", "invoker_name");
    
    //регистрация С-closure
    
  lua_pushlightuserdata (state, &invoker);
  lua_pushstring        (state, invoker_name);  //??????
  lua_pushcclosure      (state, &invoke_dispatch, 2);
  lua_setglobal         (state, invoker_name);
}

void Interpreter::UnregisterInvoker (const char* invoker_name)
{
  if (!invoker_name)
    return;

  lua_pushnil    (state);
  lua_setglobal  (state, invoker_name);
}
    
/*
    Регистрация / удаление реестров
*/

void Interpreter::RegisterInvokerRegistry (const char* registry_name, InvokerRegistry& registry)
{
  if (!registry_name)
    RaiseNullArgument ("script::lua::RegisterInvokerRegistry", "registry_name");     
    
    //проверка: является ли регистрируемый реестр глобальным

  if (!strcmp ("global", registry_name))
  {
    RegisterGlobalRegistry (registry);
  }
  else
  {    
      //регистрация мета-таблицы
      
    Metatable* metatable = new Metatable (state, registry_name, registry);
    
    try
    {
      metatables.insert_pair (registry_name, metatable);
    }
    catch (...)
    {
      delete metatable;
      throw;
    }
  }
}

void Interpreter::UnregisterInvokerRegistry (const char* registry_name)
{
  if (!registry_name)
    return;
       
  if (!strcmp ("global", registry_name))
  {
    on_register_invoker_connection.disconnect ();
    on_unregister_invoker_connection.disconnect ();
  }
  else
  {
      //удаление метатаблицы      

    MetatableMap::iterator iter = metatables.find (registry_name);

    if (iter == metatables.end ())
      return;

    delete iter->second;

    metatables.erase (iter);
  }
}

void Interpreter::RegisterGlobalRegistry (InvokerRegistry& registry)
{
    //регистрация обработчиков событий

  on_register_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnRegisterInvoker,
      bind (&Interpreter::RegisterInvoker, this, _2, _3));

  on_unregister_invoker_connection = registry.RegisterEventHandler (InvokerRegistryEvent_OnUnregisterInvoker,
      bind (&Interpreter::UnregisterInvoker, this, _2));
      
    //регистрация шлюзов
    
  for (InvokerRegistry::Iterator i=registry.CreateIterator (); i; ++i)
    RegisterInvoker (registry.InvokerId (i), *i);
}

/*
    Поиск метатаблицы
*/

Metatable* Interpreter::FindMetatable (const char* name) const
{
  if (!name)
    return 0;
    
  MetatableMap::const_iterator iter = metatables.find (name);
  
  if (iter == metatables.end ())
    return 0;
    
  return iter->second;
}

namespace script
{

/*
    Создание интерпретатора lua
*/

xtl::com_ptr<IInterpreter> create_lua_interpreter (const xtl::shared_ptr<Environment>& environment)
{
  if (!environment)
    common::RaiseNullArgument ("script::create_lua_interpreter", "environment");

  return xtl::com_ptr<IInterpreter> (new Interpreter (environment), false);
}

}
