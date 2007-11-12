#include "shared.h"

using namespace script;
using namespace common;

/*
    Описание реализации скриптового окружения
*/

namespace
{

struct NamedRegistry
{
  stl::string     name;     //имя реестра
  InvokerRegistry registry; //реестр

  NamedRegistry (const char* in_name) : name (in_name) {}    
};

}

typedef stl::hash_map<stl::hash_key<const char*>, NamedRegistry*>           RegistryMap;
typedef xtl::signal<void (EnvironmentEvent, const char*, InvokerRegistry&)> EnvironmentSignal;

struct Environment::Impl
{
  RegistryMap       registries;                       //реестры
  EnvironmentSignal handlers [EnvironmentEvent_Num];  //сигналы
  
  Impl () {}
  Impl (const Impl& impl) : registries (impl.registries) {}
  
  void Notify (EnvironmentEvent event_id, const char* name, InvokerRegistry& registry)
  {
    if (!handlers [event_id])
      return;
      
    try
    {      
      handlers [event_id] (event_id, name, registry);
    }
    catch (...)
    {
      //подавляем все исключения
    }
  }  
};

/*
    Конструкторы / деструктор
*/

Environment::Environment ()
  : impl (new Impl)
  {}

Environment::Environment (const Environment& environment)
  : impl (new Impl (*environment.impl))
  {}

Environment::~Environment ()
{
  Clear ();
}

/*
    Присваивание
*/

Environment& Environment::operator = (const Environment& environment)
{
  Environment (environment).Swap (*this);
  
  return *this;
}

/*
    Создание / удаление / поиск реестров
*/

InvokerRegistry& Environment::CreateRegistry (const char* id)
{
  if (!id)
    RaiseNullArgument ("script::Environment::CreateRegistry", "id");

  RegistryMap::const_iterator iter = impl->registries.find (id);

  if (iter != impl->registries.end ())
    RaiseInvalidArgument ("script::Environment::CreateRegistry", "id", id, "Registry with this name already registered");

  NamedRegistry* named_registry = new NamedRegistry (id);

  try
  {
    impl->registries.insert_pair (id, named_registry);
  }
  catch (...)
  {
    delete named_registry;
    throw;
  }

  impl->Notify (EnvironmentEvent_OnCreateRegistry, id, named_registry->registry);

  return named_registry->registry;
}

void Environment::RemoveRegistry (const char* id)
{
  if (!id)
    return;
    
  RegistryMap::iterator iter = impl->registries.find (id);
  
  if (iter == impl->registries.end ())
    return;

  impl->Notify (EnvironmentEvent_OnRemoveRegistry, id, iter->second->registry);

  delete iter->second;

  impl->registries.erase (iter);
}

InvokerRegistry* Environment::FindRegistry (const char* id) const
{
  if (!id)
    return 0;
    
  RegistryMap::iterator iter = impl->registries.find (id);
  
  if (iter == impl->registries.end ())
    return 0;

  return &iter->second->registry;
}

/*
    Очистка
*/

void Environment::Clear ()
{
    //оповещение об удалении реестров
    
  for (RegistryMap::iterator iter = impl->registries.begin (), end = impl->registries.end (); iter != end; ++iter)
    impl->Notify (EnvironmentEvent_OnRemoveRegistry, iter->second->name.c_str (), iter->second->registry);

    //удаление шлюзов
    
  for (RegistryMap::iterator iter = impl->registries.begin (), end = impl->registries.end (); iter != end; ++iter)
    delete iter->second;

  impl->registries.clear ();  
}
    
/*
    Создание итераторов
*/

namespace
{

struct RegistrySelector
{
  template <class T> InvokerRegistry& operator () (T& value) const { return value.second->registry; }
};

}

Environment::Iterator Environment::CreateIterator ()
{
  return Iterator (impl->registries.begin (), impl->registries.begin (), impl->registries.end (), RegistrySelector ());
}

Environment::ConstIterator Environment::CreateIterator () const
{
  return ConstIterator (impl->registries.begin (), impl->registries.begin (), impl->registries.end (), RegistrySelector ());
}

/*
    Получение имени реестра по итератору
*/

const char* Environment::RegistryId (const ConstIterator& i) const
{
  const RegistryMap::iterator* iter = i.target<RegistryMap::iterator> ();

  if (!iter)
    common::RaiseInvalidArgument ("script::Environment::RegistryId", "iterator", "wrong-type");

  return (*iter)->second->name.c_str ();
}

/*
    Регистрация обработчиков событий
*/

xtl::connection Environment::RegisterEventHandler (EnvironmentEvent event_id, const EventHandler& handler)
{
  if (event_id < 0 || event_id >= EnvironmentEvent_Num)
    common::RaiseInvalidArgument ("script::Environment::RegisterEventHandler", "event_id", event_id);

  return impl->handlers [event_id].connect (handler);
}

/*
    Обмен
*/

void Environment::Swap (Environment& environment)
{
  impl.swap (environment.impl);
}

namespace script
{

void swap (Environment& env1, Environment& env2)
{
  env1.Swap (env2);
}

}
