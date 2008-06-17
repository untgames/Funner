#include "shared.h"

using namespace script;

/*
    Именованный шлюз
*/

namespace
{

struct NamedInvoker
{
  Invoker     invoker; //шлюз
  stl::string name;    //имя шлюза
  
  NamedInvoker (const char* in_name, const Invoker& in_invoker) : invoker (in_invoker), name (in_name) {}
};

}

/*
    Описание реализации реестра шлюзов
*/

typedef stl::hash_map<stl::hash_key<const char*>, NamedInvoker>         InvokerMap;
typedef xtl::signal<void (InvokerRegistryEvent, const char*, Invoker&)> RegistrySignal;

struct InvokerRegistry::Impl
{
  InvokerMap     invokers;                            //карта шлюзов
  RegistrySignal handlers [InvokerRegistryEvent_Num]; //обработчики событий
  
  Impl () {}
  Impl (const Impl& impl) : invokers (impl.invokers) {}
  
  void Notify (InvokerRegistryEvent event_id, const char* invoker_name, Invoker& invoker)
  {
    if (!handlers [event_id])
      return;
      
    try
    {      
      handlers [event_id] (event_id, invoker_name, invoker);
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

InvokerRegistry::InvokerRegistry ()
  : impl (new Impl)
{  
}

InvokerRegistry::InvokerRegistry  (const InvokerRegistry& registry)
  : impl (new Impl (*registry.impl))
  {}

InvokerRegistry::~InvokerRegistry ()
{
  Clear ();  
}

/*
    Присваивание
*/

InvokerRegistry& InvokerRegistry::operator = (const InvokerRegistry& registry)
{
  InvokerMap (registry.impl->invokers).swap (impl->invokers);

  return *this;
}

/*
    Поиск шлюза по имени
*/

const Invoker* InvokerRegistry::Find (const char* name) const
{
  if (!name)
    return 0;
    
  InvokerMap::const_iterator iter = impl->invokers.find (name);
  
  return iter != impl->invokers.end () ? &iter->second.invoker : 0;
}

/*
    Получение итератора
*/

namespace
{

struct InvokerSelector
{
  template <class T> Invoker& operator () (T& value) const { return value.second.invoker; }
};

}

InvokerRegistry::Iterator InvokerRegistry::CreateIterator ()
{
  return Iterator (impl->invokers.begin (), impl->invokers.begin (), impl->invokers.end (), InvokerSelector ());
}

InvokerRegistry::ConstIterator InvokerRegistry::CreateIterator () const
{
  return ConstIterator (impl->invokers.begin (), impl->invokers.begin (), impl->invokers.end (), InvokerSelector ());
}

/*
    Получение идентификатора шлюза
*/

const char* InvokerRegistry::InvokerId (const ConstIterator& i) const
{
  const InvokerMap::iterator* iter = i.target<InvokerMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("script::InvokerRegistry::InvokerId", "iterator", "wrong-type");

  return (*iter)->second.name.c_str ();
}

/*
    Регистрация/удаление шлюзов
*/

void InvokerRegistry::Register (const char* name, const Invoker& invoker)
{
  if (!name)
    throw xtl::make_null_argument_exception ("script::InvokerRegistry::Register", "name");
    
  Unregister (name);

  stl::pair<InvokerMap::iterator, bool> insert_result = impl->invokers.insert_pair (name, NamedInvoker (name, invoker));
  
  impl->Notify (InvokerRegistryEvent_OnRegisterInvoker, name, insert_result.first->second.invoker);
}

void InvokerRegistry::Register (const char* name, const InvokerRegistry& source_registry, const char* source_name)
{
  if (!source_name)
    throw xtl::make_null_argument_exception ("script::InvokerRegistry::Register", "source_name");
    
  const Invoker* invoker = source_registry.Find (source_name);
    
  if (!invoker)
    throw xtl::make_argument_exception ("script::InvokerRegistry::Register", "source_name", source_name, "No invoker found");
    
  Register (name, *invoker);
}

void InvokerRegistry::Register (const char* name, const char* source_name)
{
  Register (name, *this, source_name);
}

void InvokerRegistry::Register (const InvokerRegistry& registry)
{
  for (InvokerMap::const_iterator i=registry.impl->invokers.begin (), end=registry.impl->invokers.end (); i!=end; ++i)
    Register (i->second.name.c_str (), i->second.invoker);
}

void InvokerRegistry::Unregister (const char* name)
{
  if (!name)
    return;
    
  InvokerMap::iterator iter = impl->invokers.find (name);
  
  if (iter == impl->invokers.end ())
    return;

  impl->Notify (InvokerRegistryEvent_OnUnregisterInvoker, name, iter->second.invoker);

  impl->invokers.erase (iter);
}

/*
    Очистка
*/

void InvokerRegistry::Clear ()
{
    //оповещение об удалении шлюзов
    
  for (InvokerMap::iterator iter = impl->invokers.begin (), end = impl->invokers.end (); iter != end; ++iter)
    impl->Notify (InvokerRegistryEvent_OnUnregisterInvoker, iter->second.name.c_str (), iter->second.invoker);
    
    //удаление шлюзов
    
  impl->invokers.clear ();
}

/*
    Регистрация обработчиков событий
*/

xtl::connection InvokerRegistry::RegisterEventHandler (InvokerRegistryEvent event_id, const EventHandler& handler) const
{
  if (event_id < 0 || event_id >= InvokerRegistryEvent_Num)
    throw xtl::make_argument_exception ("script::InvokerRegistry::RegisterEventHandler", "event_id", event_id);

  return impl->handlers [event_id].connect (handler);
}

/*
    Обмен
*/

void InvokerRegistry::Swap (InvokerRegistry& registry)
{
  impl.swap (registry.impl);
}

namespace script
{

void swap (InvokerRegistry& registry1, InvokerRegistry& registry2)
{
  registry1.Swap (registry2);
}

}
