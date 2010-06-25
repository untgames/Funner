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
    Resource (const char* in_name, IResourceDestroyListener* in_destroy_listener)
      : name (in_name)
      , state (ResourceState_Unloaded)
      , destroy_listener (in_destroy_listener)
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
        for (ResourceArray::reverse_iterator iter=resources.rbegin (); iter!=resources.rend (); ++iter)
        {
          Resource& resource = **iter;

          if (resource.State () > ResourceState_Unloaded && resource.use_count () == 1)
          {
            do_query (&servers [0], servers.size (), resource.Name (), &ICustomServer::UnloadResource, true, true);
            resource.SetState (ResourceState_Unloaded);
          }
        }
      }
      catch (...)
      {
        //подавление всех исключений
      }

        //удаление из списка групп связывания

      group_bindings.erase (group_bindings_pos);
    }    

///Загрузка группы ресурсов
    void AsyncLoad (AsyncOperation& async_operation)
    {
      async_operation.AddEstimateSteps (resources.size () * 2);
      
      async_operation.AddTask (CreateTask (ResourceState_Prefetched));      
      async_operation.AddTask (CreateTask (ResourceState_Loaded));
    }

///Выгрузка группы ресурсов
    void AsyncUnload (AsyncOperation& async_operation)
    {
      async_operation.AddEstimateSteps (resources.size ());
      
      async_operation.AddTask (CreateTask (ResourceState_Unloaded));
    }
    
///Синхронная выгрузка ресурсов
    void SyncUnload ()
    {      
      for (ResourceArray::reverse_iterator iter=resources.rbegin (), end=resources.rend ();iter != end; ++iter)
      {
        Resource& resource = **iter;       

        try
        {
          if (resource.State () > ResourceState_Unloaded)
          {
            do_query (&servers [0], servers.size (), resource.Name (), &ICustomServer::UnloadResource, true, true);

            resource.SetState (ResourceState_Unloaded);
          }
        }
        catch (...)
        {         
        }      
      }
    }

///Получение массива ресурсов
    ResourceArray& Resources () { return resources; }
    
///Оповещение об удалении
    xtl::trackable* GetTrackable () { return this; }
    
  private:      
///Обработчик загрузки ресурсов
    template <class Fn, class Pred, class Iter> class AsyncResourceProcessing
    {
      public:
        AsyncResourceProcessing (GroupBinding* in_group, const char* in_stage, Iter in_first, Iter in_last, ResourceState in_state, Fn in_fn, Pred in_predicate, bool in_ignore_exceptions, bool in_reverse_order)
          : group (in_group)
          , stage (in_stage)
          , first (in_first)
          , last (in_last)
          , unprocessed_count (stl::distance (first, last))
          , state (in_state)
          , predicate (in_predicate)
          , fn (in_fn)
          , ignore_exceptions (in_ignore_exceptions)
          , reverse_order (in_reverse_order)
        {
        }

        bool operator () (AsyncOperation& async_operation)
        {
          if (!group)
          {
            async_operation.AddFinishedSteps (unprocessed_count);
            
            return true;
          }
          
          if (first == last)
            return true;
            
          async_operation.SetStage (stage.c_str ());

          bool processed = false; 
           
          for (;first != last && !processed; ++first)
          {
            Resource& resource = **first;
           
            unprocessed_count--;
            
            try
            {
              if (predicate (resource.State (), state))
              {
                async_operation.SetResource (resource.Name ());
        
                async_operation.UpdateProgress ();

                do_query (&group->servers [0], group->servers.size (), resource.Name (), fn, ignore_exceptions, reverse_order);
                
                resource.SetState (state);
                
                processed = true;
              }
            }
            catch (...)
            {
              async_operation.AddFinishedSteps (1);
              
              if (!ignore_exceptions)
                throw;
            }

            async_operation.AddFinishedSteps (1);
          }          
 
          return first == last;
        }
        
      private:
        typedef xtl::trackable_ptr<GroupBinding> GroupBindingPtr;

      private:
        GroupBindingPtr group;
        stl::string     stage;
        Iter            first, last;
        size_t          unprocessed_count;
        ResourceState   state;
        Pred            predicate;
        Fn              fn;
        bool            ignore_exceptions;
        bool            reverse_order;
    };
    
    AsyncOperation::TaskHandler CreateTask (ResourceState state)
    {
      switch (state)
      {
        case ResourceState_Unloaded:
          return AsyncResourceProcessing<void (ICustomServer::*)(const char*), stl::greater<ResourceState>, ResourceArray::reverse_iterator>
            (this, "Unloading", resources.rbegin (), resources.rend (), state, &ICustomServer::UnloadResource, stl::greater<ResourceState> (), true, true);
        case ResourceState_Loaded:
          return AsyncResourceProcessing<void (ICustomServer::*)(const char*), stl::less<ResourceState>, ResourceArray::iterator>
            (this, "Loading", resources.begin (), resources.end (), state, &ICustomServer::LoadResource, stl::less<ResourceState> (), false, false);
        case ResourceState_Prefetched:
          return AsyncResourceProcessing<void (ICustomServer::*)(const char*), stl::less<ResourceState>, ResourceArray::iterator>
            (this, "Prefetching", resources.begin (), resources.end (), state, &ICustomServer::PrefetchResource, stl::less<ResourceState> (), true, false);
        default:
          throw xtl::make_argument_exception ("media::rms::GroupBinding::CreateTask", "state", state);
      }
    }

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
  ServerList          servers;         //сервер управления ресурсами (ссылка без владения)
  stl::string         name;            //имя сервера
  stl::string         filters_string;  //строка, содержащая фильтры обрабатываемых ресурсов
  common::StringArray filters;         //массив фильтров новых ресурсов
  ResourceMap         resources;       //ресурсы
  GroupBindingList    group_bindings;  //список групп связывания
  bool                cache_state;     //используется ли кэш ресурсов

///Конструктор
  Impl (const char* in_name)
    : name (in_name)
    , cache_state (DEFAULT_CACHE_STATE)
  {
    servers.reserve (SERVER_ARRAY_RESERVE_SIZE);    
      
    UpdateFilters ("*");

    ResourceManagerSingleton::Instance ()->RegisterServerGroup (name.c_str (), this);
  }
  
///Деструктор
  ~Impl ()
  {
      //отмена регистрации в менеджере ресурсов

    try
    {
      ResourceManagerSingleton::Instance ()->UnregisterServerGroup (this);
    }
    catch (...)
    {
    }

      //досрочное принудительное удаление групп связывания

    while (!group_bindings.empty ())
    {
      GroupBinding* binding = group_bindings.back ();

      binding->SyncUnload ();
      
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
  struct AsyncFlushUnusedResourcesProcessing
  {
    AsyncFlushUnusedResourcesProcessing (Impl* in_impl)
      : impl (in_impl)
      , initialized (false)
    {
    }
    
    bool operator () (AsyncOperation& async_operation)
    {
      async_operation.SetStage ("Flushing");
      
      if (!impl->cache_state) //работа метода имеет смысл только в случае включенного кэширования неиспользуемых ресурсов
        return true;
      
      if (!initialized)
      {
          //построение массива обрабатываемых ресурсов
        
        resources.reserve (impl->resources.size ());
        
        for (ResourceMap::reverse_iterator iter = impl->resources.rbegin (); iter != impl->resources.rend (); ++iter) 
        {
          Resource& resource = *iter->second;

          if (resource.use_count () == 1) //если ресурс находится только в кеше
            resources.push_back (&resource);
        }

        async_operation.AddEstimateSteps (resources.size ());

        this->iter  = resources.begin ();
        initialized = true;
      }
      
      if (iter == resources.end ())
        return true;
        
      Resource& resource = **iter++;

      if (resource.State () > ResourceState_Unloaded)
      {
        try
        {
          const char* resource_name = resource.Name ();          
          
          async_operation.SetResource (resource_name);

          async_operation.UpdateProgress ();          

          do_query (&impl->servers [0], impl->servers.size (), resource_name, &ICustomServer::UnloadResource, true, true);
        }
        catch (...)
        {
          //подавление всех исключений
        }
      }

      release (&resource);

      async_operation.AddFinishedSteps (1);
      
      return false;
    }
    
    xtl::intrusive_ptr<Impl> impl;
    bool                     initialized;
    ResourceArray            resources;
    ResourceArray::iterator  iter;
  };
  
  void AsyncFlushUnusedResources (AsyncOperation& async_operation)
  {
    async_operation.AddTask (AsyncFlushUnusedResourcesProcessing (this));
  }

///Синхронная версия метода выгрузки неиспользуемых ресурсов
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
  
///Создание связывания
  Binding CreateBinding (const Group& group)
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

        for (size_t j=0, filter_count=filters.Size (); j<filter_count; j++)
        {
          const char* filter = filters [j];

          if (!common::wcimatch (resource_name, filter))
            continue;

            //имя ресурса соответствует фильтру

            //поиск ресурса среди уже зарегистрированных

          ResourceMap::iterator iter = resources.find (resource_name);

          if (iter != resources.end ())
          {
              //ресурс уже присутствует в карте - добавление ресурса в группу

            group_resources.push_back (iter->second);
          }
          else
          {
              //создание ресурса

            ResourcePtr resource (new Resource (resource_name, this), false);

              //добавление ресурса в группу

            group_resources.push_back (resource);

              //регистрация ресурса

            resources.insert_pair (resource_name, resource.get ());
          }
        }
      }

        //создание группы связывания

      GroupBinding* group_binding = new GroupBinding (servers, group_resources, group_bindings);

      Binding binding (group_binding);

        //дополнительное увеличение числа ссылок новых ресурсов для возможности кэширования

      if (cache_state)
      {
        for (ResourceArray::iterator iter=group_binding->Resources ().begin (), end=group_binding->Resources ().end (); iter!=end; ++iter)
        {
          Resource& resource = **iter;

          if (resource.use_count () == 1) //если ресурс захвачен только новой группой
          {
              //увеличиваем число ссылок на ресурс для возможности сохранения ресурса в кеше после удаления

            addref (resource);
          }
        }
      }

      return binding;
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("media::rms::ServerGroup::Impl::CreateBinding");
      throw;
    }    
  }
  
///Получение экземпляра
  ServerGroup Instance ()
  {
    return this;
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
      
    IServerGroupInstance* instance = ResourceManagerSingleton::Instance ()->FindServerGroup (name);    
    
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
    return impl->CreateBinding (group);
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

    impl->FlushUnusedResources ();    

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
  try
  {
    AsyncOperation::Pointer async_operation = AsyncOperation::Create ();

    impl->AsyncFlushUnusedResources (*async_operation);
  }
  catch (...)
  {
  }
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
