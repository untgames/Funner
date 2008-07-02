#ifndef COMMONLIB_VAR_REGISTRY_TESTS_SHARED_HEADER
#define COMMONLIB_VAR_REGISTRY_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/string>
#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <xtl/signal.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/lexical_cast.h>

#include <common/var_registry.h>
#include <common/var_registry_container.h>

using namespace common;

//тестовый пользовательский реестр переменных
class TestVarRegistry: public ICustomVarRegistry, public xtl::reference_counter
{
  public:
    TestVarRegistry ()
    {
      printf ("TestVarRegistry::TestVarRegistry()\n");
    }
    
    ~TestVarRegistry ()
    {
      printf ("TestVarRegistry::~TestVarRegistry\n");
    }
  
///Получение/установка данных
    xtl::any GetValue (const char* var_name)
    {
      static const char* METHOD_NAME = "TestVarRegistry::GetValue";

      if (!var_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "var_name");

      VarMap::iterator iter = vars.find (var_name);

      if (iter == vars.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "var_name", var_name, "Variable not found");              

      return iter->second;
    }
    
    void SetValue (const char* var_name, const xtl::any& value)
    {
      static const char* METHOD_NAME = "TestVarRegistry::SetValue";

      if (!var_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "var_name");

      VarMap::iterator iter = vars.find (var_name);
      
      if (iter == vars.end ())
      {
        vars.insert_pair (var_name, value);

        Notify (var_name, VarRegistryEvent_OnCreateVar);

        return;
      }            

      vars [var_name] = value;

      Notify (var_name, VarRegistryEvent_OnChangeVar);
    }
    
///Удаление переменной
    void RemoveVariable (const char* var_name)
    {
      if (!var_name)
        return;

      VarMap::iterator iter = vars.find (var_name);

      if (iter == vars.end ())
        return;

      vars.erase (iter);

      Notify (var_name, VarRegistryEvent_OnDeleteVar);
    }

///Проверка наличия переменной
    bool HasVariable (const char* var_name)
    {
      if (!var_name)
        return false;

      return vars.count (var_name) != 0;
    }

///Обход всех переменных
    void EnumerateVars (const EnumHandler& handler)
    {
      for (VarMap::iterator iter=vars.begin (); iter!=vars.end (); ++iter)
        handler (iter->first.c_str ());
    }

///Добавление/удаление ссылки
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }

///Подписка на добавление/изменение/удаление переменных
    void SetEventHandler (const EventHandler& handler)
    {
      event_handler = handler;
    }

    const EventHandler& GetEventHandler ()
    {
      return event_handler;
    }
    
///Создание
    typedef xtl::com_ptr<TestVarRegistry> Pointer;

    static Pointer Create ()
    {
      return Pointer (new TestVarRegistry, false);
    }

  private:
    TestVarRegistry (const TestVarRegistry&); //no impl
    TestVarRegistry& operator = (const TestVarRegistry&); //no impl
    
    void Notify (const char* var_name, VarRegistryEvent event)
    {
      try
      {
        event_handler (var_name, event);
      }
      catch (...)
      {
      }
    }
    
  private:
    typedef stl::hash_map<stl::string, xtl::any> VarMap;

  private:  
    VarMap       vars;
    EventHandler event_handler;   
};

#endif
