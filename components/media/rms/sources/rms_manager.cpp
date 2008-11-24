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
    void AddBinding (const Group& group, Server& server)
    {
      server_bindins.push_back (ServerBinding (group, server));
    }
    
///Удаление связывания с сервером
    void RemoveBinding (Server& server)
    {
      for (ServerBindingList::iterator iter=server_bindins.begin (), end=server_bindins.end (); iter!=end;)
      {
        if (iter->server == &server)
        {
          ServerBindingList::iterator next = iter;

          ++next;

          server_bindins.erase (iter);

          iter = next;
        }
        else ++iter;
      }
    }
    
///Предвыборка ресурсов
    void Prefetch ()
    {
      for (ServerBindingList::iterator iter=server_bindins.begin (), end=server_bindins.end (); iter!=end; ++iter)
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
      for (ServerBindingList::iterator iter=server_bindins.begin (), end=server_bindins.end (); iter!=end; ++iter)
      {
        try
        {
          iter->binding.Load ();
        }
        catch (...)
        {
          //подавление всех исключений
        }
      }      
    }
    
///Выгрузка ресурсов
    void Unload ()
    {
      for (ServerBindingList::iterator iter=server_bindins.begin (), end=server_bindins.end (); iter!=end; ++iter)
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
      Binding binding;
      Server* server;

      ServerBinding (const Group& group, Server& in_server) : binding (in_server.CreateBinding (group)), server (&in_server) {}
    };
    
    typedef stl::list<ServerBinding> ServerBindingList;
    
  private:
    ServerBindingList            server_bindins; //список связываний групп ресурсов с сервером
    ManagerBindingList&          bindings;       //список связываний
    ManagerBindingList::iterator bindings_pos;   //положение в списке связываний
};

/*
    Сервер, присоединенный к менеджеру ресурсов
*/

struct ServerAttachment: public xtl::reference_counter
{
  Server&              server;            //ссылка на сервер
  xtl::auto_connection on_destroy_server; //соединение с сигналом, оповещающем об удалении сервера  

///Конструктор
  ServerAttachment (Server& in_server, xtl::auto_connection& in_on_destroy_server)
    : server (in_server)
  {
    on_destroy_server.swap (in_on_destroy_server);
  }
};

typedef xtl::intrusive_ptr<ServerAttachment> ServerAttachmentPtr;

}

/*
    Описание реализации менеджера ресурсов
*/

typedef stl::list<ServerAttachmentPtr> ServerList;

struct ResourceManager::Impl: public xtl::reference_counter
{
  ManagerBindingList bindings; //список связываний
  ServerList         servers;  //список серверов

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

ResourceManager::ResourceManager ()
  : impl (new Impl)
{
}

ResourceManager::ResourceManager (const ResourceManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ResourceManager::~ResourceManager ()
{
  release (impl);
}

ResourceManager& ResourceManager::operator = (const ResourceManager& manager)
{
  ResourceManager (manager).Swap (*this);
  
  return *this;
}

/*
    Связывание с серверами ресурсов
*/

Binding ResourceManager::CreateBinding (const Group& group)
{
  try
  {
    ManagerBinding* manager_binding = new ManagerBinding (impl->bindings);
    Binding         binding (manager_binding);

    for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
      manager_binding->AddBinding (group, (*iter)->server);

    return binding;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rms::ResourceManager::CreateBinding");
    throw;
  }
}

/*
    Управление серверами ресурсов
*/

void ResourceManager::Attach (Server& server)
{
    //проверка повторной регистрации
    
  for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
    if (&(*iter)->server == &server)
      return;

    //подписка на событие удаления сервера
    
  xtl::auto_connection on_destroy_server = server.GetTrackable ().connect_tracker (xtl::bind (&ResourceManager::Detach, this, xtl::ref (server)));

    //добавление сервера

  impl->servers.push_back (ServerAttachmentPtr (new ServerAttachment (server, on_destroy_server), false));
}

void ResourceManager::Detach (Server& server)
{
  for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
    if (&(*iter)->server == &server)
    {
        //удаление связываний с данным сервером
        
      for (ManagerBindingList::iterator binding=impl->bindings.begin (), binding_end=impl->bindings.end (); binding!=binding_end; ++binding)
      {
        (*binding)->RemoveBinding (server);
      }

        //удаление сервера из списка серверов

      impl->servers.erase (iter);

      return;
    }
}

/*
    Сброс неиспользуемых ресурсов
*/

void ResourceManager::FlushUnusedResources ()
{
  for (ServerList::iterator iter=impl->servers.begin (), end=impl->servers.end (); iter!=end; ++iter)
    (*iter)->server.FlushUnusedResources ();
}

/*
    Обмен
*/

void ResourceManager::Swap (ResourceManager& manager)
{
  stl::swap (impl, manager.impl);
}

namespace media
{

namespace rms
{

void swap (ResourceManager& manager1, ResourceManager& manager2)
{
  manager1.Swap (manager2);
}

}

}
