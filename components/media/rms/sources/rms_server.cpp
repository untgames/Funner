#include "shared.h"

using namespace media::rms;

namespace
{

/*
    Константы
*/

const size_t MAX_QUERY_NAMES             = 64;    //максимальное количество ресурсов, передаваемых в запросе к ICustomServer
const size_t RESOURCE_ARRAY_RESERVE_SIZE = 128;   //резервируемое число ресурсов в группе ресурсов
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
    Запрос к серверу ресурсов
*/

class ServerQuery
{
  public:
    typedef void (ICustomServer::*CallbackHandler)(size_t count, const char** resource_names);
    
///Конструктор
    ServerQuery (ICustomServer& in_server, CallbackHandler in_callback) : server (in_server), callback (in_callback), names_count (0) {}

///Добавление имени ресурса к запросу
    void Add (const char* name)
    {
      if (names_count == MAX_QUERY_NAMES)
        DoQuery ();

      names [names_count] = name;

      names_count++;
    }

///Выполнение запроса
    void DoQuery ()
    {
      if (!names_count)
        return;
      
      (server.*callback)(names_count, names);

      names_count = 0;
    }

  private:
    ICustomServer&  server;                  //целевой сервер ресурсов
    CallbackHandler callback;                //обработчик обратного вызова
    size_t          names_count;             //количество имён ресурсов в запросе
    const char*     names [MAX_QUERY_NAMES]; //имена ресурсов в запросе
};

/*
    Связывание с группой ресурсов
*/

class GroupBinding;

typedef stl::list<GroupBinding*> GroupBindingList;

class GroupBinding: public ICustomBinding, public xtl::trackable
{
  public:
///Конструктор
    GroupBinding (ICustomServer& in_server, ResourceArray& in_resources, GroupBindingList& in_group_bindings)
      : server (in_server),
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

        ServerQuery query (server, &ICustomServer::UnloadResources);

        SetResourcesState (ResourceState_Unloaded, query, DestructorPredicate ());
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
      ServerQuery query (server, &ICustomServer::PrefetchResources);

      SetResourcesState (ResourceState_Prefetched, query, StatePredicate<stl::less<ResourceState> > ());
    }

///Загрузка группы ресурсов
    void Load ()
    {
      ServerQuery query (server, &ICustomServer::LoadResources);
      
      SetResourcesState (ResourceState_Loaded, query, StatePredicate<stl::less<ResourceState> > ());
    }

///Выгрузка группы ресурсов
    void Unload ()
    {
      ServerQuery query (server, &ICustomServer::UnloadResources);

      SetResourcesState (ResourceState_Unloaded, query, StatePredicate<stl::greater<ResourceState> > ());
    }

///Получение объекта, оповещающего о досрочном удалении GroupBinding
    xtl::trackable* GetTrackable () { return this; }
    
///Получение массива ресурсов
    ResourceArray& Resources () { return resources; }

  private:
    template <class Pred> void SetResourcesState (ResourceState state, ServerQuery& query, Pred pred)
    {
        //оповещение сервера ресурсов

      for (ResourceArray::iterator iter=resources.begin (), end=resources.end (); iter!=end; ++iter)
      {
        Resource& resource = **iter;
        
        if (pred (resource, state))
          query.Add (resource.Name ());
      }

      query.DoQuery ();
      
        //установка состояний ресурсов
      
      for (ResourceArray::iterator iter=resources.begin (), end=resources.end (); iter!=end; ++iter)
      {
        Resource& resource = **iter;
        
        if (pred (resource, state))
          resource.SetState (state);
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
    ICustomServer&             server;             //сервер ресурсов
    ResourceArray              resources;          //ресурсы
    GroupBindingList&          group_bindings;     //список групп связывания
    GroupBindingList::iterator group_bindings_pos; //положение в списке групп связывания
};

}

/*
    Описание реализации сервера ресурсов
*/

struct Server::Impl: public IResourceDestroyListener, public xtl::trackable
{
  ICustomServer*      server;         //сервер управления ресурсами (ссылка без владения)
  stl::string         name;           //имя сервера
  stl::string         filters_string; //строка, содержащая фильтры обрабатываемых ресурсов
  common::StringArray filters;        //массив фильтров новых ресурсов
  ResourceMap         resources;      //ресурсы
  GroupBindingList    group_bindings; //список групп связывания
  bool                cache_state;    //используется ли кэш ресурсов

///Конструктор
  Impl (ICustomServer& in_server)
    : server (&in_server),
      cache_state (DEFAULT_CACHE_STATE)
  {
    UpdateFilters ("*");
  }
  
///Деструктор
  ~Impl ()
  {
      //досрочное принудительное удаление групп связывания

    while (!group_bindings.empty ())
    {
      GroupBinding* binding = group_bindings.front ();
      
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

        server->UnloadResources (1, &resource_name);
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

    ServerQuery query (*server, &ICustomServer::UnloadResources);

    for (ResourceMap::iterator iter=resources.begin (), end=resources.end (); iter!=end;)
    {
      Resource& resource = *iter->second;

      if (resource.use_count () == 1) //если ресурс находится только в кеше
      {
        if (resource.State () > ResourceState_Unloaded)
        {
          try
          {
            query.Add (resource.Name ());
          }
          catch (...)
          {
            //подавление всех исключений
          }

          resource.SetState (ResourceState_Unloaded);
        }

        ++iter;

        release (&resource);
      }
      else ++iter;
    }
    
    try
    {
      query.DoQuery ();
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

Server::Server (ICustomServer& server)
  : impl (new Impl (server))
{
}

Server::~Server ()
{
}

/*
    Имя сервера
*/

const char* Server::Name () const
{
  return impl->name.c_str ();
}

void Server::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rms::Server::SetName", "name");

  impl->name = name;
}

/*
    Фильтры имён обрабатываемых ресурсов
*/

const char* Server::Filters () const
{
  return impl->filters_string.c_str ();
}

void Server::SetFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::Server::SetFilters", "filters_string");
    
  impl->UpdateFilters (filters_string);
}

void Server::AddFilters (const char* filters_string)
{
  if (!filters_string)
    throw xtl::make_null_argument_exception ("media::rms::Server::AddFilters", "filters_string");

  stl::string new_filters_string = impl->filters_string + ' ' + filters_string;

  impl->UpdateFilters (new_filters_string.c_str ());
}

void Server::ResetFilters ()
{
  impl->filters_string.clear ();
  impl->filters.Clear ();
}

/*
    Создание связывания с группой ресурсов
*/

Binding Server::CreateBinding (const Group& group)
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

          ResourcePtr resource (new Resource (resource_name, impl.get ()), false);

            //добавление ресурса в группу

          group_resources.push_back (resource);

            //регистрация ресурса

          impl->resources.insert_pair (resource_name, resource.get ());
        }
      }
    }
    
      //создание группы связывания

    GroupBinding* group_binding = new GroupBinding (*impl->server, group_resources, impl->group_bindings);

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
    exception.touch ("media::rms::Server::CreateBinding");
    throw;
  }
}

/*
    Управление кэшированием ресурсов
*/

void Server::SetCacheState (bool state)
{
  if (state == impl->cache_state)
    return;
    
  if (impl->cache_state)
  {
      //отключение кэша - сбрасываем кэш ресурсов, уменьшаем число ссылок всех оставшихся ресурсов
    
    FlushUnusedResources ();    

    for (ResourceMap::iterator iter=impl->resources.begin (), end=impl->resources.end (); iter!=end;)
    {
      Resource* resource = iter->second;

      ++iter;

      release (iter->second);
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

bool Server::CacheState () const
{
  return impl->cache_state;
}

/*
    Сброс неиспользуемых ресурсов
*/

void Server::FlushUnusedResources ()
{
  impl->FlushUnusedResources ();
}

/*
    Получение объекта, оповещающего об удалении сервера
*/

xtl::trackable& Server::GetTrackable () const
{
  return *impl;
}

namespace media
{

namespace rms
{

xtl::trackable& get_trackable (const Server& server)
{
  return server.GetTrackable ();
}

}

}
