#include "shared.h"

using namespace media::rms;

namespace
{

/*
    Константы
*/

const size_t RESOURCE_ARRAY_RESERVE_SIZE = 128;   //резервируемое число ресурсов в группе ресурсов
const size_t SERVER_ARRAY_RESERVE_SIZE   = 8;     //резервируемое число серверов в группе серверов
const bool   DEFAULT_CACHE_STATE         = false; //состояние кэша ресурсов по умолчанию

/*
    Интерфейс оповещения об удалении ресурса
*/

class IResourceDestroyListener
{
  public:
    virtual void OnDestroyResource (const char* name) {}

  protected:
    virtual ~IResourceDestroyListener () {}
};

/*
    Работа со списокм серверов ресурсов
*/

template <class Fn> void do_query (ICustomServer** servers, size_t count, const char* resource_name, const Fn& fn, bool ignore_exceptions, bool reverse_order = false)
{
  if (ignore_exceptions)
  {
    if (reverse_order)
    {
      for (size_t i=0; i<count; i++)
      {
        try
        {
          (servers [count-i-1]->*fn)(resource_name);
        }
        catch (...)
        {
          //игнорирование исключений
        }
      }      
    }
    else
    {
      for (size_t i=0; i<count; i++)
      {
        try
        {
          (servers [i]->*fn)(resource_name);
        }
        catch (...)
        {
          //игнорирование исключений
        }
      }
    }
  }
  else
  {
    try
    {
      if (reverse_order)
      {
        for (size_t i=0; i<count; i++)
        {
          (servers [i]->*fn)(resource_name);
        }
      }
      else
      {
        for (size_t i=0; i<count; i++)
        {
          (servers [count-i-1]->*fn)(resource_name);
        }        
      }
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("media::rms::do_query");
      throw;
    }
  }
}

/*
    Описание ресурса
*/

enum ResourceState
{
  ResourceState_Unloaded,   //ресурс выгружен
  ResourceState_Prefetched, //ресурс предвыбран
  ResourceState_Loaded      //ресурс загружен
};

class Resource: public xtl::noncopyable, public xtl::reference_counter
{
  public:
///Конструктор
    Resource (const char* in_name, IResourceDestroyListener* in_destroy_listener) :
      name (in_name),
      state (ResourceState_Unloaded),
      destroy_listener (in_destroy_listener)
    {}

///Деструктор
    ~Resource ()
    {
      if (destroy_listener)
        destroy_listener->OnDestroyResource (name.c_str ());
    }

///Имя ресурса
    const char* Name () const { return name.c_str (); }

///Состояние ресурса
    ResourceState State () const { return state; }

    void SetState (ResourceState new_state) { state = new_state; }

  private:
    stl::string               name;             //имя ресурса
    ResourceState             state;            //состояние ресурса
    IResourceDestroyListener* destroy_listener; //слушатель удаления ресурса
};

typedef xtl::intrusive_ptr<Resource>                         ResourcePtr;
typedef stl::vector<ResourcePtr>                             ResourceArray;
typedef stl::hash_map<stl::hash_key<const char*>, Resource*> ResourceMap; //weak-ref

/*
    Связывание с группой ресурсов
*/

class GroupBinding;

typedef stl::list<GroupBinding*>    GroupBindingList;
typedef stl::vector<ICustomServer*> ServerList;

class GroupBinding: public ICustomBinding, public xtl::trackable
{
  public:
///Конструктор
    GroupBinding (ServerList& in_servers, ResourceArray& in_resources, GroupBindingList& in_group_bindings)
      : servers (in_servers),
        group_bindings (in_group_bindings)
    {
        //инициализация массива ресурсов группы

      resources.swap (in_resources);

        //добавление в список групп связывания

      group_bindings_pos = group_bindings.insert (group_bindings.end (), this);
    }

///Деструктор
    ~GroupBinding ()
    {
      try
      {
          //оповещение о групповой выгрузке ресурсов со ссылкой только на данную группу

        SetResourcesState (ResourceState_Unloaded, &ICustomServer::UnloadResource, DestructorPredicate (), true, true);
      }
      catch (...)
      {
        //подавление всех исключений
      }

        //удаление из списка групп связывания

      group_bindings.erase (group_bindings_pos);
    }

///Предвыборка группы ресурсов
    void Prefetch ()
    {
      SetResourcesState (ResourceState_Prefetched, &ICustomServer::PrefetchResource, StatePredicate<stl::less<ResourceState> > (), true);
    }

///Загрузка группы ресурсов
    void Load ()
    {
      SetResourcesState (ResourceState_Loaded, &ICustomServer::LoadResource, StatePredicate<stl::less<ResourceState> > (), false);
    }

///Выгрузка группы ресурсов
    void Unload ()
    {
      SetResourcesState (ResourceState_Unloaded, &ICustomServer::UnloadResource, StatePredicate<stl::greater<ResourceState> > (), true, true);
    }

///Получение объекта, оповещающего о досрочном удалении GroupBinding
    xtl::trackable* GetTrackable () { return this; }

///Получение массива ресурсов
    ResourceArray& Resources () { return resources; }
        
  private:
    template <class Fn, class Pred, class Iter> void SetResourcesState (ResourceState state, Fn fn, Pred pred, bool ignore_exceptions, bool reverse_order, Iter first, Iter last)
    {
      for (; first!=last; ++first)        
      {
        Resource& resource = **first;

        if (pred (resource, state))
        {
          do_query (&servers [0], servers.size (), resource.Name (), fn, ignore_exceptions, reverse_order);
          resource.SetState (state);
        }
      }
    }
  
    template <class Fn, class Pred> void SetResourcesState (ResourceState state, Fn fn, Pred pred, bool ignore_exceptions, bool reverse_order = false)
    {
      if (reverse_order)
      {
        SetResourcesState (state, fn, pred, ignore_exceptions, reverse_order, resources.rbegin (), resources.rend ());
      }
      else
      {
        SetResourcesState (state, fn, pred, ignore_exceptions, reverse_order, resources.begin (), resources.end ());
      }
    }

    template <class Pred> struct StatePredicate
    {
      bool operator () (const Resource& resource, ResourceState state) const { return Pred () (resource.State (), state); }
    };

    struct DestructorPredicate
    {
      bool operator () (const Resource& resource, ResourceState state) const
      {
        return resource.State () > state && resource.use_count () == 1;
      }
    };

  private:
    ServerList&                servers;            //список серверов ресурсов
    ResourceArray              resources;          //ресурсы
    GroupBindingList&          group_bindings;     //список групп связывания
    GroupBindingList::iterator group_bindings_pos; //положение в списке групп связывания
};

}

/*
    Описание реализации сервера ресурсов
*/

struct ServerGroup::Impl: public IResourceDestroyListener, public IServerGroupInstance, public xtl::reference_counter
{
  ServerList          servers;        //сервер управления ресурсами (ссылка без владения)
  stl::string         name;           //имя сервера
  stl::string         filters_string; //строка, содержащая фильтры обрабатываемых ресурсов
  common::StringArray filters;        //массив фильтров новых ресурсов
  ResourceMap         resources;      //ресурсы
  GroupBindingList    group_bindings; //список групп связывания
  bool                cache_state;    //используется ли кэш ресурсов

///Конструктор
  Impl (const char* in_name)
    : name (in_name)
    , cache_state (DEFAULT_CACHE_STATE)
  {
    servers.reserve (SERVER_ARRAY_RESERVE_SIZE);    
      
    UpdateFilters ("*");

    ResourceManagerSingleton::Instance ().RegisterServerGroup (name.c_str (), this);
  }
  
///Деструктор
  ~Impl ()
  {
      //отмена регистрации в менеджере ресурсов

    try
    {
      ResourceManagerSingleton::Instance ().UnregisterServerGroup (this);
    }
    catch (...)
    {
    }

      //досрочное принудительное удаление групп связывания

    while (!group_bindings.empty ())
    {
      GroupBinding* binding = group_bindings.back ();

      binding->Unload ();

      delete binding;
    }

      //сброс кэша ресурсов

    FlushUnusedResources ();    
  }

///Обновление массива фильтров новых ресурсов
  void UpdateFilters (const char* str)
  {
    common::StringArray new_filters = common::split (str);

    filters_string = str;

    filters.Swap (new_filters);
  }

///Оповещение об удалении ресурса
  void OnDestroyResource (const char* name)
  {
    ResourceMap::iterator iter = resources.find (name);

    if (iter == resources.end ())
      return;

    if (iter->second->State () > ResourceState_Unloaded)
    {
      try
      {
        const char* resource_name = iter->second->Name ();

        do_query (&servers [0], servers.size (), resource_name, &ICustomServer::UnloadResource, true, true);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

    resources.erase (iter);
  }

///Сброс неиспользуемых ресурсов
  void FlushUnusedResources ()
  {
      //работа метода имеет смысл только в случае включенного кэширования неиспользуемых ресурсов

    if (!cache_state)
      return;

    for (ResourceMap::reverse_iterator iter = resources.rbegin (); iter != resources.rend ();)
    {
      Resource& resource = *iter->second;      

      if (resource.use_count () == 1) //если ресурс находится только в кеше
      {
        ResourceMap::reverse_iterator next = iter;        

        ++++next;
        
        if (resource.State () > ResourceState_Unloaded)
        {
          do_query (&servers [0], servers.size (), resource.Name (), &ICustomServer::UnloadResource, true, true);

          resource.SetState (ResourceState_Unloaded);
        }

        release (&resource);
        
        iter = --next;
      }
      else ++iter;
    }
  }
  
///Имя группы
  const char* Name ()
  {
    return name.c_str ();
  }
  
///Получение экземпляра
  ServerGroup Instance ()
  {
    return ServerGroup (this);
  }
  
///Подсчёт ссылок
  void AddRef ()
  {
    addref (this);
  }
  
  void Release ()
  {
    release (this);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

ServerGroup::ServerGroup (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

ServerGroup::ServerGroup (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    IServerGroupInstance* instance = ResourceManagerSingleton::Instance ().FindServerGroup (name);    
    
    if (instance)
    {
      impl = instance->Instance ().impl;

      addref (impl);
    }
    else
    {
      impl = new Impl (name);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::ServerGroup");
    throw;
  }
}

ServerGroup::ServerGroup (const ServerGroup& group)
  : impl (group.impl)
{
  addref (impl);
}

ServerGroup::~ServerGroup ()
{
  release (impl);
}

ServerGroup& ServerGroup::operator = (const ServerGroup& group)
{
  ServerGroup (group).Swap (*this);

  return *this;
}

/*
    Имя группы
*/

const char* ServerGroup::Name () const
{
  return impl->name.c_str ();
}

void ServerGroup::SetName (const char* name)
{  
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    ServerGroup (name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::SetName");
    throw;
  }
}

/*
    Добавление / удаление серверов ресурсов
*/

void ServerGroup::Attach (ICustomServer& server)
{
  try
  {
      //проверка повторного добавления

    for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
      if (*iter == &server)
        return;

      //восстановление состояния ресурсов
      
    ICustomServer* server_ptr = &server;      

    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end; ++iter)
    {
      Resource& resource = *iter->second;

      if (resource.State () == ResourceState_Loaded)
        do_query (&server_ptr, 1, resource.Name (), &ICustomServer::LoadResource, false);
    }

      //добавление сервера

    impl->servers.push_back (&server);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::Attach");
    throw;
  }
}

void ServerGroup::Detach (ICustomServer& server)
{
  for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
    if (*iter == &server)
    {
        //выгрузка ресурсов с сервера
        
      ICustomServer* server_ptr = &server;        
             
      for (ResourceMap::reverse_iterator res_iter=impl->resources.rbegin (), res_end=impl->resources.rend (); res_iter!=res_end; ++res_iter)
      {
        Resource& resource = *res_iter->second;

        if (resource.State () > ResourceState_Unloaded)
          do_query (&server_ptr, 1, resource.Name (), &ICustomServer::UnloadResource, true);
      }

        //удаление сервера из списка серверов
      
      impl->servers.erase (iter);

      return;
    }
}

/*
    Фильтры имён обрабатываемых ресурсов
*/

const char* ServerGroup::Filters () const
{
  return impl->filters_string.c_str ();
}

void ServerGroup::SetFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::ServerGroup::SetFilters", "filters_string");

  impl->UpdateFilters (filters_string);
}

void ServerGroup::AddFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::ServerGroup::AddFilters", "filters_string");

  stl::string new_filters_string = impl->filters_string + ' ' + filters_string;

  impl->UpdateFilters (new_filters_string.c_str ());
}

void ServerGroup::ResetFilters ()
{
  impl->filters_string.clear ();
  impl->filters.Clear ();
}

/*
    Создание связывания с группой ресурсов
*/

Binding ServerGroup::CreateBinding (const Group& group)
{
  try
  {
      //создание массива ресурсов группы

    ResourceArray group_resources;

    group_resources.reserve (RESOURCE_ARRAY_RESERVE_SIZE);

        //создание группы ресурсов

    for (size_t i=0, group_count=group.Size (); i<group_count; i++)
    {
      const char* resource_name = group.Item (i);

      for (size_t j=0, filter_count=impl->filters.Size (); j<filter_count; j++)
      {
        const char* filter = impl->filters [j];

        if (!common::wcimatch (resource_name, filter))
          continue;

          //имя ресурса соответствует фильтру

          //поиск ресурса среди уже зарегистрированных

        ResourceMap::iterator iter = impl->resources.find (resource_name);

        if (iter != impl->resources.end ())
        {
            //ресурс уже присутствует в карте - добавление ресурса в группу

          group_resources.push_back (iter->second);
        }
        else
        {
            //создание ресурса

          ResourcePtr resource (new Resource (resource_name, impl), false);

            //добавление ресурса в группу

          group_resources.push_back (resource);

            //регистрация ресурса

          impl->resources.insert_pair (resource_name, resource.get ());
        }
      }
    }

      //создание группы связывания

    GroupBinding* group_binding = new GroupBinding (impl->servers, group_resources, impl->group_bindings);

    Binding binding (group_binding);

      //дополнительное увеличение числа ссылок новых ресурсов для возможности кэширования

    if (impl->cache_state)
    {
      for (ResourceArray::iterator iter=group_binding->Resources ().begin (), end=group_binding->Resources ().end (); iter!=end; ++iter)
      {
        Resource& resource = **iter;

        if (resource.use_count () == 1) //если ресурс захвачен только новой группой
        {
            //увеличиваем число ссылок не ресурс для возможности сохранения ресурса в кеше после удаления

          addref (resource);
        }
      }
    }

    return binding;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ServerGroup::CreateBinding");
    throw;
  }
}

/*
    Управление кэшированием ресурсов
*/

void ServerGroup::SetCacheState (bool state)
{
  if (state == impl->cache_state)
    return;

  if (impl->cache_state)
  {
      //отключение кэша - сбрасываем кэш ресурсов, уменьшаем число ссылок всех оставшихся ресурсов

    FlushUnusedResources ();    

    for (ResourceMap::reverse_iterator iter=impl->resources.rbegin (), end=impl->resources.rend (); iter!=end;)
    {
      Resource* resource = iter->second;

      ++iter;

      release (resource);
    }
  }
  else
  {
      //включение кэша - увеличиваем число ссылок всех ресурсов

    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end; ++iter)
      addref (iter->second);
  }

  impl->cache_state = state;
}

bool ServerGroup::CacheState () const
{
  return impl->cache_state;
}

/*
    Сброс неиспользуемых ресурсов
*/

void ServerGroup::FlushUnusedResources ()
{
  impl->FlushUnusedResources ();
}

/*
    Обмен
*/

void ServerGroup::Swap (ServerGroup& group)
{
  stl::swap (impl, group.impl);
}

namespace media
{

namespace rms
{

void swap (ServerGroup& group1, ServerGroup& group2)
{
  group1.Swap (group2);
}

}

}
