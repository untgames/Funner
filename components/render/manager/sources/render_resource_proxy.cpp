#include "shared.h"

using namespace render;

namespace render
{

/*
===================================================================================================
    ResourceProxyManagerImpl
===================================================================================================
*/

template <class Ptr> struct ResourceProxyManagerImpl: public xtl::reference_counter
{
  typedef Ptr                                                   Pointer;
  typedef ResourceProxyImpl<Pointer>                            ProxyImpl;
  typedef stl::hash_map<stl::hash_key<const char*>, ProxyImpl*> ProxyMap;
  typedef typename ProxyMap::iterator                           ProxyMapIterator;

  ProxyMap proxies;          //прокси объекты
  Pointer  default_resource; //ресурс по умолчанию
};

/*
===================================================================================================
    ResourceProxy
===================================================================================================
*/

template <class Ptr> struct ResourceProxyImpl: public xtl::reference_counter, public CacheHolder
{
  typedef Ptr                             Pointer;
  typedef ResourceProxyManagerImpl<Ptr>   ManagerImpl;
  typedef typename ManagerImpl::ProxyMap  ProxyMap;
  typedef typename ProxyMap::iterator     ProxyMapIterator;
  typedef xtl::intrusive_ptr<ManagerImpl> ResourceProxyManagerPtr;

  Pointer                 resource;       //хранимый объект
  ResourceProxyManagerPtr manager;        //ссылка на менеджер
  stl::string             name;           //имя ресурса
  bool                    is_default;     //является ли ресурс дэфолтным
  ProxyMapIterator        proxy_position; //положение прокси объекта в списке менеджера
  
///Конструктор
  ResourceProxyImpl (ManagerImpl& in_manager, const char* in_name)
    : manager (&in_manager)
    , name (in_name ? in_name : "")
    , is_default (false)
    , proxy_position (manager->proxies.end ())
  {
  }
  
///Деструктор
  ~ResourceProxyImpl ()
  {
    if (proxy_position != manager->proxies.end ())
      manager->proxies.erase (proxy_position);
  }
  
///Переопределение методов кэша
  void UpdateCacheCore ()
  {
  }
  
  void ResetCacheCore ()
  {
  }
  
  using CacheHolder::Invalidate;
};

}

/*
    Конструкторы / деструктор / присваивание
*/

template <class Ptr>
ResourceProxy<Ptr>::ResourceProxy (ManagerImpl& manager, const char* name)
  : impl (new Impl (manager, name))
{
  try
  {
    if (!impl->name.empty ())
    {
      stl::pair<typename Impl::ProxyMapIterator, bool> result = impl->manager->proxies.insert_pair (impl->name.c_str (), impl);

      if (!result.second)
        throw xtl::format_operation_exception ("render::ResourceProxy<Ptr>::ResourceProxy", "Internal error: resource with name '%s' already exists", impl->name.c_str ());
              
      impl->proxy_position = result.first;      
    }      
    
    SetDefaultResource (manager.default_resource);
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

template <class Ptr>
ResourceProxy<Ptr>::ResourceProxy (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
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
bool ResourceProxy<Ptr>::IsDefaultResource ()
{
  return impl->is_default;
}

/*
    Получение данных
*/

template <class Ptr>
typename ResourceProxy<Ptr>::Pointer ResourceProxy<Ptr>::Resource ()
{
  if (impl->resource)
    return impl->resource;
    
  throw xtl::format_operation_exception ("render::ResourceProxy<Ptr>::Resource", "Resource '%s' not found. Default resource hasn't set", impl->name.c_str ());
}

namespace
{

template <class T, T t> struct SfinaeHelper {};

template <class T>
void detach_cache_holder (T& resource, CacheHolder& proxy, SfinaeHelper<void (T::*)(CacheHolder&), &T::Detach>* = 0)
{
  proxy.DetachCacheSource (resource);
}

template <class T>
void detach_cache_holder (T& resource, CacheHolder& proxy, ...)
{
}

template <class T>
void attach_cache_holder (T& resource, CacheHolder& proxy, SfinaeHelper<void (T::*)(CacheHolder&), &T::Attach>* = 0)
{
  proxy.AttachCacheSource (resource);
}

template <class T>
void attach_cache_holder (T& resource, CacheHolder& proxy, ...)
{
}

}

template <class Ptr>
void ResourceProxy<Ptr>::SetResource (const Pointer& ptr)
{
  if (ptr)
    attach_cache_holder (*ptr, *impl);

  if (impl->resource)
    detach_cache_holder (*impl->resource, *impl);

  impl->resource   = ptr;
  impl->is_default = !ptr;
  
  impl->Invalidate ();
}

template <class Ptr>
const char* ResourceProxy<Ptr>::Name ()
{
  return impl->name.c_str ();
}

template <class Ptr>
void ResourceProxy<Ptr>::SetDefaultResource (const Pointer& ptr)
{
  SetResource (ptr);

  impl->is_default = true;
}

/*
    Связывание со списком обновления кэша
*/

template <class Ptr>
void ResourceProxy<Ptr>::AttachCacheHolder (CacheHolder& holder)
{
  holder.AttachCacheSource (*impl);
}

template <class Ptr>
void ResourceProxy<Ptr>::DetachCacheHolder (CacheHolder& holder)
{
  holder.DetachCacheSource (*impl);
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
  release (impl);
}

/*
    Получение прокси
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
    Поиск ресурса
*/

template <class Ptr>
typename ResourceProxyManager<Ptr>::Pointer ResourceProxyManager<Ptr>::FindResource (const char* name)
{
  if (!name)
    return Pointer ();
    
    //поиск прокси в кэше
    
  typedef typename Impl::ProxyMapIterator ProxyMapIterator;
  
  ProxyMapIterator iter = impl->proxies.find (name);
  
  if (iter != impl->proxies.end ())
  {
    if (iter->second->is_default)
      return Pointer ();
    
    return iter->second->resource;
  }
    
  return Pointer ();
}

/*
    Установка объекта по умолчанию
*/

template <class Ptr>
void ResourceProxyManager<Ptr>::SetDefaultResource (const Pointer& ptr)
{
  impl->default_resource = ptr;

  typedef typename Impl::ProxyMapIterator ProxyMapIterator;
  
  for (ProxyMapIterator iter=impl->proxies.begin (); iter!=impl->proxies.end (); ++iter)
  {
    if (iter->second->is_default)
      Proxy (iter->second).SetDefaultResource (ptr);
  }
}

template <class Ptr>
typename ResourceProxyManager<Ptr>::Pointer ResourceProxyManager<Ptr>::DefaultResource ()
{
  return impl->default_resource;
}

/*
    Инстанцирование
*/

template class ResourceProxy<TexturePtr>;
template class ResourceProxy<LowLevelSamplerStatePtr>;
template class ResourceProxy<MaterialPtr>;
template class ResourceProxy<PrimitivePtr>;
template class ResourceProxyManager<TexturePtr>;
template class ResourceProxyManager<LowLevelSamplerStatePtr>;
template class ResourceProxyManager<MaterialPtr>;
template class ResourceProxyManager<PrimitivePtr>;
