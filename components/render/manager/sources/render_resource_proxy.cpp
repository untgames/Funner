#include "shared.h"

using namespace render;

namespace render
{

/*
===================================================================================================
    ResourceProxyManagerImpl
===================================================================================================
*/

template <class Ptr> struct ResourceProxyManagerImpl
{
  typedef Ptr                                              Pointer;
  typedef ResourceProxy<Pointer>                           Proxy;
  typedef stl::hash_map<stl::hash_key<const char*>, Proxy> ProxyMap;
  
  ProxyMap proxies;        //прокси объекты
  Pointer  default_object; //ресурс по умолчанию
};

}

/*
===================================================================================================
    ResourceProxy
===================================================================================================
*/

template <class Ptr> struct ResourceProxy<Ptr>::Impl: public xtl::reference_counter, public CacheHolder
{
  typedef typename ManagerImpl::ProxyMap ProxyMap;
  typedef typename ProxyMap::iterator    ProxyMapIterator;

  Pointer      object;         //хранимый объект
  ManagerImpl& manager;        //ссылка на менеджер
  stl::string  name;           //имя ресурса
  bool         is_default;     //является ли ресурс дэфолтным
  
///Конструктор
  Impl (ManagerImpl& in_manager, const char* in_name)
    : manager (in_manager)
    , name (in_name ? in_name : "")
    , is_default (false)
  {
  }
  
///Кэшируемый источник требует обновления кэша.
  void UpdateCacheCore ()
  {
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

template <class Ptr>
ResourceProxy<Ptr>::ResourceProxy (ManagerImpl& manager, const char* name)
  : impl (new Impl (manager, name))
{
  try
  {
    stl::pair<typename Impl::ProxyMapIterator, bool> result = impl->manager.proxies.insert_pair (impl->name.c_str (), *this);

    if (!result.second)
      throw xtl::format_operation_exception ("render::ResourceProxy<Ptr>::ResourceProxy", "Internal error: resource with name '%s' already exists", impl->name.c_str ());
      
    SetDefaultData (manager.default_object);
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

template <class Ptr>
ResourceProxy<Ptr>::ResourceProxy (const ResourceProxy& proxy)
  : impl (proxy.impl)
{
  addref (impl);
}

template <class Ptr>
ResourceProxy<Ptr>::~ResourceProxy ()
{
  release (impl);
}

template <class Ptr>
ResourceProxy<Ptr>& ResourceProxy<Ptr>::operator = (const ResourceProxy& proxy)
{
  ResourceProxy tmp (proxy);
  
  stl::swap (impl, tmp.impl);
  
  return *this;
}

/*
    Проверка корректности связанных данных
*/

template <class Ptr>
bool ResourceProxy<Ptr>::IsDefaultData ()
{
  return impl->is_default;
}

/*
    Получение данных
*/

template <class Ptr>
typename ResourceProxy<Ptr>::Pointer ResourceProxy<Ptr>::Data ()
{
  if (impl->object)
    return impl->object;
    
  throw xtl::format_operation_exception ("render::ResourceProxy<Ptr>::Data", "Resource '%s' not found. Default object hasn't set", impl->name.c_str ());
}

namespace
{

template <class T, T t> struct SfinaeHelper {};

template <class T>
void detach_cache_holder (T& object, CacheHolder& child, SfinaeHelper<void (T::*)(CacheHolder&), &T::Detach>* = 0)
{
  object.Detach (child);
}

template <class T>
void detach_cache_holder (T& object, CacheHolder& child, ...)
{
}

template <class T>
void attach_cache_holder (T& object, CacheHolder& child, SfinaeHelper<void (T::*)(CacheHolder&), &T::Attach>* = 0)
{
  object.Attach (child);
}

template <class T>
void attach_cache_holder (T& object, CacheHolder& child, ...)
{
}

}

template <class Ptr>
void ResourceProxy<Ptr>::SetData (const Pointer& ptr)
{
  if (ptr)
    attach_cache_holder (*ptr, *impl);

  if (impl->object)
    detach_cache_holder (*impl->object, *impl);

  impl->object     = ptr;
  impl->is_default = false;    
}

template <class Ptr>
const char* ResourceProxy<Ptr>::Name ()
{
  return impl->name.c_str ();
}

template <class Ptr>
void ResourceProxy<Ptr>::SetDefaultData (const Pointer& ptr)
{
  SetData (ptr);

  impl->is_default = true;
}

/*
    Связывание со списком обновления кэша
*/

template <class Ptr>
void ResourceProxy<Ptr>::Attach (CacheHolder& holder)
{
  impl->Attach (holder);
}

template <class Ptr>
void ResourceProxy<Ptr>::Detach (CacheHolder& holder)
{
  impl->Detach (holder);
}

/*
===================================================================================================
    ResourceProxyManager
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

template <class Ptr>
ResourceProxyManager<Ptr>::ResourceProxyManager ()
  : impl (new Impl)
{
}

template <class Ptr>
ResourceProxyManager<Ptr>::~ResourceProxyManager ()
{
}

/*
    Создание прокси
*/

template <class Ptr>
typename ResourceProxyManager<Ptr>::Proxy ResourceProxyManager<Ptr>::GetProxy (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::ResourceProxyManager::GetProxy", "name");
    
    //поиск прокси в кэше
    
  typedef typename Impl::ProxyMapIterator ProxyMapIterator;
  
  ProxyMapIterator iter = impl->proxies.find (name);
  
  if (iter != impl->proxies.end ())
    return iter->second;
    
    //создание нового прокси
    
  return Proxy (*impl, name);
}

/*
    Установка объекта по умолчанию
*/

template <class Ptr>
void ResourceProxyManager<Ptr>::SetDefault (const Pointer& ptr)
{
  impl->default_object = ptr;
  
  for (ProxyMapIterator iter=impl->proxies.begin (); iter!=impl->proxies.end (); ++iter)
  {
    if (iter->second->IsDefaultData ())
      iter->second->SetDefaultData (ptr);
  }
}

template <class Ptr>
typename ResourceProxyManager<Ptr>::Pointer ResourceProxyManager<Ptr>::Default ()
{
  return impl->default_object;
}

/*
    Инстанцирование
*/

template class ResourceProxy<TexturePtr>;
template class ResourceProxy<MaterialPtr>;
template class ResourceProxy<PrimitivePtr>;
