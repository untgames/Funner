#include "shared.h"

using namespace media::rms;

namespace
{

/*
    Связывание менеджера с группами ресурсов
*/

class ManagerBinding;

typedef stl::list<ManagerBinding*> ManagerBindingList;

class ManagerBinding: public ICustomBinding, public xtl::trackable
{
  public:
///Конструктор
    ManagerBinding (ManagerBindingList& in_bindings) : bindings (in_bindings)
    {
        //регистрация в списке связываний

      bindings_pos = bindings.insert (bindings.end (), this);
    }

///Деструктор
    ~ManagerBinding ()
    {
      bindings.erase (bindings_pos);
    }

///Добавление связывания группы ресурсов с сервером
    void AddBinding (const Group& group, IServerGroupInstance* server_group)
    {
      server_bindings.push_back (ServerBinding (group, server_group));
    }

///Удаление связывания с сервером
    void RemoveBinding (IServerGroupInstance* server_group)
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end;)
      {
        if (iter->server_group == server_group)
        {
          ServerBindingList::iterator next = iter;

          ++next;

          server_bindings.erase (iter);

          iter = next;
        }
        else ++iter;
      }
    }

///Предвыборка ресурсов
    void Prefetch ()
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end; ++iter)
      {
        try
        {
          iter->binding.Prefetch ();
        }
        catch (...)
        {
          //подавление всех исключений
        }
      }
    }

///Загрузка ресурсов
    void Load ()
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end; ++iter)
      {
          //исключения загрузки не подавляются

        iter->binding.Load ();
      }
    }

///Выгрузка ресурсов
    void Unload ()
    {
      for (ServerBindingList::iterator iter=server_bindings.begin (), end=server_bindings.end (); iter!=end; ++iter)
      {
        try
        {
          iter->binding.Unload ();
        }
        catch (...)
        {
          //подавление всех исключений
        }
      }
    }

///Получение объекта, оповещающего об удалении текущего объекта (может быть 0)
    xtl::trackable* GetTrackable () { return this; }

  private:
    struct ServerBinding
    {
      Binding               binding;
      IServerGroupInstance* server_group;

      ServerBinding (const Group& resource_group, IServerGroupInstance* in_server_group)
        : binding (in_server_group->Instance ().CreateBinding (resource_group))
        , server_group (in_server_group)
      {
      }
    };

    typedef stl::list<ServerBinding> ServerBindingList;

  private:
    ServerBindingList            server_bindings; //список связываний групп ресурсов с сервером
    ManagerBindingList&          bindings;       //список связываний
    ManagerBindingList::iterator bindings_pos;   //положение в списке связываний
};

}

/*
    Описание реализации менеджера ресурсов
*/

typedef stl::vector<IServerGroupInstance*> ServerGroupList;

struct ResourceManagerImpl::Impl
{
  ManagerBindingList bindings; //список связываний
  ServerGroupList    groups;   //список групп

///Деструктор
  ~Impl ()
  {
    while (!bindings.empty ())
      delete bindings.front ();
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

ResourceManagerImpl::ResourceManagerImpl ()
  : impl (new Impl)
{
}

ResourceManagerImpl::~ResourceManagerImpl ()
{
}

/*
    Связывание с серверами ресурсов
*/

Binding ResourceManagerImpl::CreateBinding (const Group& group)
{
  try
  {
    ManagerBinding* manager_binding = new ManagerBinding (impl->bindings);
    Binding         binding (manager_binding);

    for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
      manager_binding->AddBinding (group, *iter);

    return binding;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ResourceManager::CreateBinding");
    throw;
  }
}

/*
    Регистрация группы ресурсов
*/

void ResourceManagerImpl::RegisterServerGroup (const char* name, IServerGroupInstance* group)
{
  impl->groups.push_back (group);
}

void ResourceManagerImpl::UnregisterServerGroup (IServerGroupInstance* group)
{
  for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
    if (*iter == group)
    {
        //удаление связываний с данной группой серверов

      for (ManagerBindingList::iterator binding=impl->bindings.begin (), binding_end=impl->bindings.end (); binding!=binding_end; ++binding)
        (*binding)->RemoveBinding (group);

        //удаление группы серверов из списка

      impl->groups.erase (iter);

      return;
    }
}

/*
    Поиск группы серверов
*/

IServerGroupInstance* ResourceManagerImpl::FindServerGroup (const char* name) const
{
  if (!name)
    return 0;

  for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
    if (!strcmp ((*iter)->Name (), name))
      return *iter;

  return 0;
}

/*
    Перебор групп
*/

size_t ResourceManagerImpl::ServerGroupsCount () const
{
  return impl->groups.size ();
}

IServerGroupInstance* ResourceManagerImpl::ServerGroup (size_t index) const
{
  if (index >= impl->groups.size ())
    throw xtl::make_range_exception ("media::rms::ResourceManagerImpl::ServerGroup", "index", index, impl->groups.size ());
    
  return impl->groups [index];
}

/*
    Сброс неиспользуемых ресурсов
*/

void ResourceManagerImpl::FlushUnusedResources ()
{
  for (ServerGroupList::iterator iter=impl->groups.begin (), end=impl->groups.end (); iter!=end; ++iter)
    (*iter)->Instance ().FlushUnusedResources ();
}

/*
    Делегирование вызовов ResourceManager
*/

Binding ResourceManager::CreateBinding (const Group& group)
{
  return ResourceManagerSingleton::Instance ().CreateBinding (group);
}

size_t ResourceManager::ServerGroupsCount ()
{
  return ResourceManagerSingleton::Instance ().ServerGroupsCount ();
}

media::rms::ServerGroup ResourceManager::ServerGroup (size_t index)
{
  return ResourceManagerSingleton::Instance ().ServerGroup (index)->Instance ();
}

void ResourceManager::FlushUnusedResources ()
{
  ResourceManagerSingleton::Instance ().FlushUnusedResources ();
}
