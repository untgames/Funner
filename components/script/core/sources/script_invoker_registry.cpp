#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const bool DEFAULT_AUTO_OVERLOADS_MODE = false; //по умолчанию автоматические перегрузки отключены

/*
    Именованный шлюз
*/

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

struct InvokerRegistry::Impl: public xtl::reference_counter
{
  InvokerMap     invokers;                            //карта шлюзов
  RegistrySignal handlers [InvokerRegistryEvent_Num]; //обработчики событий
  bool           auto_overloads_mode;                 //режим автоматических перегрузок
  
  Impl () : auto_overloads_mode (DEFAULT_AUTO_OVERLOADS_MODE) {}
  Impl (const Impl& impl) : invokers (impl.invokers), auto_overloads_mode (impl.auto_overloads_mode) {}
  
  ~Impl ()
  {
    Clear ();
  }
  
  void Clear ()
  {
      //оповещение об удалении шлюзов
      
    for (InvokerMap::iterator iter = invokers.begin (), end = invokers.end (); iter != end; ++iter)
      Notify (InvokerRegistryEvent_OnUnregisterInvoker, iter->second.name.c_str (), iter->second.invoker);
      
      //удаление шлюзов
      
    invokers.clear ();
  }

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

InvokerRegistry::InvokerRegistry (Impl* in_impl)
  : impl (in_impl)
{
}

InvokerRegistry::InvokerRegistry  (const InvokerRegistry& registry)
  : impl (registry.impl)
{
  addref (impl);
}

InvokerRegistry::~InvokerRegistry ()
{
  release (impl);
}

/*
    Клонирование
*/

InvokerRegistry InvokerRegistry::Clone () const
{
  try
  {
    return InvokerRegistry (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("script::InvokerRegistry::Clone");
    throw;
  }
}

/*
    Идентификатор
*/

size_t InvokerRegistry::Id () const
{
  return reinterpret_cast<size_t> (impl);
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
    Режим автоматических перегрузок
*/

void InvokerRegistry::SetAutoOverloadsMode (bool state)
{
  impl->auto_overloads_mode = state;
}

bool InvokerRegistry::AutoOverloadsMode () const
{
  return impl->auto_overloads_mode;
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
    
  InvokerMap::iterator iter = impl->invokers.find (name);
  
  if (iter == impl->invokers.end ())
  {
    stl::pair<InvokerMap::iterator, bool> insert_result = impl->invokers.insert_pair (name, NamedInvoker (name, invoker));
  
    impl->Notify (InvokerRegistryEvent_OnRegisterInvoker, name, insert_result.first->second.invoker);
  }
  else
  {
    if (impl->auto_overloads_mode)
    {    
      iter->second.invoker.AddOverloads (invoker);
    
      impl->Notify (InvokerRegistryEvent_OnUnregisterInvoker, name, iter->second.invoker);
      impl->Notify (InvokerRegistryEvent_OnRegisterInvoker, name, iter->second.invoker);          
    }
    else
    {
      Unregister (name);
      
      stl::pair<InvokerMap::iterator, bool> insert_result = impl->invokers.insert_pair (name, NamedInvoker (name, invoker));
  
      impl->Notify (InvokerRegistryEvent_OnRegisterInvoker, name, insert_result.first->second.invoker);      
    }
  }
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

//регистрация копий шлюзов registry
void InvokerRegistry::Register (const InvokerRegistry& registry)
{
  for (InvokerMap::const_iterator i=registry.impl->invokers.begin (), end=registry.impl->invokers.end (); i!=end; ++i)
    Register (i->second.name.c_str (), i->second.invoker);
}

//регистрация копий шлюзов registry
void InvokerRegistry::Register (const Environment& environment, const char* registry_name)
{
  try
  {
    if (!registry_name)
      throw xtl::make_null_argument_exception ("", "registry_name");
      
    InvokerRegistry* registry = environment.FindLibrary (registry_name);
    
    if (!registry)
      throw xtl::make_argument_exception ("", "registry_name", registry_name, "No registry with this name");

    Register (*registry);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::InvokerRegistry::Register(const Environment&, const char*)");
    throw;
  }
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
  impl->Clear ();
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
  stl::swap (impl, registry.impl);
}

namespace script
{

void swap (InvokerRegistry& registry1, InvokerRegistry& registry2)
{
  registry1.Swap (registry2);
}

}
