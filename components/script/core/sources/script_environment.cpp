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

struct TypeImpl: public Environment::Type, public xtl::reference_counter
{
  LibraryImpl*          library;   //ссылка на библиотеку
  const std::type_info* std_type;  //стандартный тип
  const xtl::type_info* ext_type;  //динамический тип  
  
  TypeImpl (LibraryImpl* in_library, const std::type_info* in_std_type, const xtl::type_info* in_ext_type)
    : library (in_library), std_type (in_std_type), ext_type (in_ext_type) {}
    
///Информация о типе
  bool HasExtendedType ()
  {
    return ext_type != 0;
  }
  
  const xtl::type_info& ExtendedType ()
  {
    if (!ext_type)
      throw xtl::format_operation_exception ("script::Environment::Type::ExtendedType", "No extended type information for type '%s'",
        std_type->name ());
    
    return *ext_type;
  }
  
  const std::type_info& StdType ()
  {
    return *std_type;
  }

///Библиотека шлюзов
  InvokerRegistry& Library ()
  {
    return library->registry;
  }
  
///Имя библиотеки типа
  const char* Name ()
  {
    return library->id.c_str ();
  }
};

typedef stl::hash_map<stl::hash_key<const char*>, LibraryImpl*>                    LibraryMap;
typedef xtl::intrusive_ptr<TypeImpl>                                               TypePtr;
typedef stl::hash_map<const std::type_info*, TypePtr>                              TypeMap;
typedef xtl::signal<void (EnvironmentLibraryEvent, const char*, InvokerRegistry&)> EnvironmentSignal;

}

struct Environment::Impl: public xtl::reference_counter
{
  LibraryMap        libraries;                               //библиотеки
  TypeMap           types;                                   //типы
  EnvironmentSignal handlers [EnvironmentLibraryEvent_Num];  //сигналы

  Impl () {}

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
    for (TypeMap::iterator i=types.begin (), end=types.end (); i!=end;)
      if (i->second->library == library)
      {
        TypeMap::iterator tmp = i;

        ++tmp;

        types.erase (i);

        i = tmp;
      }
      else ++i;
  }
  
  void RegisterType (const char* library_id, const std::type_info* std_type, const xtl::type_info* ext_type)
  {
    if (!library_id)
      throw xtl::make_null_argument_exception ("script::Environment::RegisterType", "library_id");
      
    LibraryMap::iterator iter = libraries.find (library_id);
    
    if (iter == libraries.end ())
      throw xtl::make_argument_exception ("script::Environment::RegisterType", "library_id", library_id, "No library with this id");

    types.insert_pair (std_type, TypePtr (new TypeImpl (iter->second, std_type, ext_type), false));
  }
  
  private:
    Impl (const Impl&);
};

/*
    Конструкторы / деструктор
*/

Environment::Environment ()
  : impl (new Impl)
  {}

Environment::Environment (const Environment& environment)
  : impl (environment.impl)
{
  addref (impl);
}

Environment::~Environment ()
{
  if (impl->decrement ())
  {
    Clear ();
    delete impl;
  }
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

InvokerRegistry Environment::CreateLibrary (const char* id)
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
  impl->types.clear ();
}

/*
    Регистрация ассоциаций
*/

void Environment::RegisterType (const std::type_info& type, const char* library_id)
{
  impl->RegisterType (library_id, &type, 0);
}

void Environment::RegisterType (const xtl::type_info& type, const char* library_id)
{
  impl->RegisterType (library_id, &type.std_type (), &type);
}

void Environment::UnregisterType (const std::type_info& type)
{   
  impl->types.erase (&type);
}

void Environment::UnregisterType (const xtl::type_info& type)
{   
  UnregisterType (type.std_type ());
}

void Environment::UnregisterAllTypes ()
{
  impl->types.clear ();
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

InvokerRegistry Environment::Library (const char* id)
{
  InvokerRegistry* registry = FindLibrary (id);
  
  if (registry)
    return *registry;

  return CreateLibrary (id);
}

const char* Environment::FindLibraryId (const std::type_info& type) const
{
  TypeMap::const_iterator iter = impl->types.find (&type);
  
  if (iter == impl->types.end ())
    return 0;
    
  return iter->second->library->id.c_str ();
}

InvokerRegistry* Environment::FindLibrary (const std::type_info& type) const
{
  return FindLibrary (FindLibraryId (type));
}

/*
    Поиск типа
*/

Environment::Type* Environment::FindType (const std::type_info& type) const
{
  TypeMap::const_iterator iter = impl->types.find (&type);
  
  if (iter == impl->types.end ())
    return 0;
    
  return &*iter->second;
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

struct TypeSelector
{
  template <class T> Environment::Type& operator () (T& value) const { return *value.second; }
};

}

Environment::LibraryIterator Environment::CreateLibraryIterator ()
{
  return LibraryIterator (impl->libraries.begin (), impl->libraries.begin (), impl->libraries.end (), RegistrySelector ());
}

Environment::ConstLibraryIterator Environment::CreateLibraryIterator () const
{
  return ConstLibraryIterator (impl->libraries.begin (), impl->libraries.begin (), impl->libraries.end (), RegistrySelector ());
}

Environment::TypeIterator Environment::CreateTypeIterator ()
{
  return TypeIterator (impl->types.begin (), impl->types.begin (), impl->types.end (), TypeSelector ());
}

Environment::ConstTypeIterator Environment::CreateTypeIterator () const
{
  return ConstTypeIterator (impl->types.begin (), impl->types.begin (), impl->types.end (), TypeSelector ());
}

/*
    Получение имени библиотеки по итератору
*/

const char* Environment::LibraryId (const ConstLibraryIterator& i) const
{
  const LibraryMap::iterator* iter = i.target<LibraryMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("script::Environment::LibraryId", "iterator", "wrong-type");

  return (*iter)->second->id.c_str ();
}

/*
    Биндинг библиотек
*/

void Environment::BindLibraries (const char* library_mask)
{
  try
  {
    LibraryManager::BindLibraries (*this, library_mask);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("script::Environment::BindLibraries");
    throw;
  }
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
  stl::swap (impl, environment.impl);
}

namespace script
{

void swap (Environment& env1, Environment& env2)
{
  env1.Swap (env2);
}

}
