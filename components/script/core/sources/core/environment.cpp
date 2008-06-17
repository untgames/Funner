#include "shared.h"

using namespace script;
using namespace common;

/*
    Описание реализации скриптового окружения
*/

namespace
{

struct LibraryImpl
{
  stl::string     id;       //идентификатор библиотеки
  InvokerRegistry registry; //реестр

  LibraryImpl (const char* in_id) : id (in_id) {}    
};

typedef stl::hash_map<stl::hash_key<const char*>, LibraryImpl*>                    LibraryMap;
typedef stl::hash_map<const std::type_info*, LibraryImpl*>                         LinkMap;
typedef xtl::signal<void (EnvironmentLibraryEvent, const char*, InvokerRegistry&)> EnvironmentSignal;

}

struct Environment::Impl
{
  LibraryMap        libraries;                               //библиотеки
  LinkMap           links;                                   //ссылки
  EnvironmentSignal handlers [EnvironmentLibraryEvent_Num];  //сигналы

  Impl () {}
  Impl (const Impl& impl) : libraries (impl.libraries), links (impl.links) {}

  void Notify (EnvironmentLibraryEvent event_id, const char* id, InvokerRegistry& registry)
  {
    if (!handlers [event_id])
      return;
      
    try
    {      
      handlers [event_id] (event_id, id, registry);
    }
    catch (...)
    {
      //подавляем все исключения
    }
  }
  
  void RemoveLinks (LibraryImpl* library)
  {
    for (LinkMap::iterator i=links.begin (), end=links.end (); i!=end;)
      if (i->second == library)
      {
        LinkMap::iterator tmp = i;

        ++tmp;

        links.erase (i);

        i = tmp;
      }
      else ++i;
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
    Создание / удаление / поиск библиотек
*/

InvokerRegistry& Environment::CreateLibrary (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("script::Environment::CreateLibrary", "id");

  LibraryMap::const_iterator iter = impl->libraries.find (id);

  if (iter != impl->libraries.end ())
    throw xtl::make_argument_exception ("script::Environment::CreateLibrary", "id", id, "Library with this id already registered");

  LibraryImpl* library = new LibraryImpl (id);

  try
  {
    impl->libraries.insert_pair (id, library);
  }
  catch (...)
  {
    delete library;
    throw;
  }

  impl->Notify (EnvironmentLibraryEvent_OnCreate, id, library->registry);

  return library->registry;
}

void Environment::RemoveLibrary (const char* id)
{
  if (!id)
    return;
    
  LibraryMap::iterator iter = impl->libraries.find (id);
  
  if (iter == impl->libraries.end ())
    return;

  impl->Notify (EnvironmentLibraryEvent_OnRemove, id, iter->second->registry);  
  impl->RemoveLinks (iter->second);

  delete iter->second;  

  impl->libraries.erase (iter);
}

void Environment::RemoveAllLibraries ()
{
    //оповещение об удалении библиотек
    
  for (LibraryMap::iterator iter = impl->libraries.begin (), end = impl->libraries.end (); iter != end; ++iter)
    impl->Notify (EnvironmentLibraryEvent_OnRemove, iter->second->id.c_str (), iter->second->registry);

    //удаление библиотек
    
  for (LibraryMap::iterator iter = impl->libraries.begin (), end = impl->libraries.end (); iter != end; ++iter)   
    delete iter->second;

  impl->libraries.clear ();
  impl->links.clear ();
}

/*
    Регистрация ассоциаций
*/

void Environment::RegisterType (const std::type_info& type, const char* library_id)
{
  if (!library_id)
    throw xtl::make_null_argument_exception ("script::Environment::RegisterType", "library_id");
    
  LibraryMap::iterator iter = impl->libraries.find (library_id);
  
  if (iter == impl->libraries.end ())
    throw xtl::make_argument_exception ("script::Environment::RegisterType", "library_id", library_id, "No library with this id");

  impl->links [&type] = iter->second;
}

void Environment::UnregisterType (const std::type_info& type)
{   
  impl->links.erase (&type);
}

void Environment::UnregisterAllTypes ()
{
  impl->links.clear ();
}

/*
    Поиск библиотеки
*/

InvokerRegistry* Environment::FindLibrary (const char* id) const
{
  if (!id)
    return 0;
    
  LibraryMap::iterator iter = impl->libraries.find (id);
  
  if (iter == impl->libraries.end ())
    return 0;

  return &iter->second->registry;
}

InvokerRegistry& Environment::Library (const char* id)
{
  InvokerRegistry* registry = FindLibrary (id);
  
  if (registry)
    return *registry;

  return CreateLibrary (id);
}

const char* Environment::FindLibraryId (const std::type_info& type) const
{
  LinkMap::const_iterator iter = impl->links.find (&type);
  
  if (iter == impl->links.end ())
    return 0;
    
  return iter->second->id.c_str ();
}

InvokerRegistry* Environment::FindLibrary (const std::type_info& type) const
{
  return FindLibrary (FindLibraryId (type));
}

/*
    Очистка
*/

void Environment::Clear ()
{
  RemoveAllLibraries ();
  UnregisterAllTypes (); 
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
  return Iterator (impl->libraries.begin (), impl->libraries.begin (), impl->libraries.end (), RegistrySelector ());
}

Environment::ConstIterator Environment::CreateIterator () const
{
  return ConstIterator (impl->libraries.begin (), impl->libraries.begin (), impl->libraries.end (), RegistrySelector ());
}

/*
    Получение имени библиотеки по итератору
*/

const char* Environment::LibraryId (const ConstIterator& i) const
{
  const LibraryMap::iterator* iter = i.target<LibraryMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("script::Environment::LibraryId", "iterator", "wrong-type");

  return (*iter)->second->id.c_str ();
}

/*
    Регистрация обработчиков событий
*/

xtl::connection Environment::RegisterEventHandler (EnvironmentLibraryEvent event_id, const EventHandler& handler)
{
  if (event_id < 0 || event_id >= EnvironmentLibraryEvent_Num)
    throw xtl::make_argument_exception ("script::Environment::RegisterEventHandler", "event_id", event_id);

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
