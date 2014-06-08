#include "shared.h"

using namespace render::scene::interchange;

namespace
{

/*
    Реализация менеджера соединений
*/

struct ConnectionCreatorResult
{
  ConnectionManager::ConnectionCreator creator;
  common::Lockable*                    lockable;

  ConnectionCreatorResult ()
    : lockable ()
  {
  }

  ~ConnectionCreatorResult ()
  {
    try
    {
      if (lockable)
        lockable->Unlock ();
    }
    catch (...)
    {
    }
  }
};

class ConnectionManagerImpl
{
  public:
    typedef ConnectionManager::ConnectionCreator ConnectionCreator;

/// Регистрация соединения
    void RegisterConnection (const char* name, const char* mask, const ConnectionCreator& creator)
    {
      static const char* METHOD_NAME = "render::scene::interchange::ConnectionManager::RegisterConnection";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!mask)
        throw xtl::make_null_argument_exception (METHOD_NAME, "mask");

      ConnectionManagerDescPtr desc (new ConnectionManagerDesc (mask, creator), false);

      if (managers.find (name) != managers.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Connection manager with such name has been already registered");

      managers.insert_pair (name, desc);
    }

/// Отмена регистрации
    void UnregisterConnection (const char* name)
    {
      if (!name)
        return;

      managers.erase (name);
    }

/// Отмена всех регистраций
    void UnregisterAllConnections ()
    {
      managers.clear ();
    }

///Получение порождающего функтора
    void GetCreator (const char* name, ConnectionCreatorResult& result)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");

        for (ConnectionManagerMap::iterator iter=managers.begin (), end=managers.end (); iter!=end; ++iter)
        {
          if (!common::wcmatch (name, iter->second->mask.c_str ()))
            continue;

          iter->second->Lock ();

          result.creator  = iter->second->creator;
          result.lockable = &*iter->second;

          return;
        }

        throw xtl::format_operation_exception ("", "Can't find connection manager for connection '%s'", name);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::interchange::ConnectionManager::GetCreator");
        throw;
      }
    }    

  private:
    struct ConnectionManagerDesc: public xtl::reference_counter, public common::Lockable
    {
      stl::string       mask;
      ConnectionCreator creator;

      ConnectionManagerDesc (const char* in_mask, const ConnectionCreator& in_creator)
        : mask (in_mask)
        , creator (in_creator)
      {
      }

      ~ConnectionManagerDesc ()
      {
        try
        {
          Lock ();
        }
        catch (...)
        {
        }     
      }
    };

    typedef xtl::intrusive_ptr<ConnectionManagerDesc>                           ConnectionManagerDescPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, ConnectionManagerDescPtr> ConnectionManagerMap;

  private:
    ConnectionManagerMap managers;
};

typedef common::Singleton<ConnectionManagerImpl> ConnectionManagerSingleton;

}

/*
    Обертки
*/

void ConnectionManager::RegisterConnection (const char* manager_name, const char* mask, const ConnectionCreator& creator)
{
  ConnectionManagerSingleton::Instance ()->RegisterConnection (manager_name, mask, creator);
}

void ConnectionManager::UnregisterConnection (const char* manager_name)
{
  ConnectionManagerSingleton::Instance ()->UnregisterConnection (manager_name);
}

void ConnectionManager::UnregisterAllConnections ()
{
  ConnectionManagerSingleton::Instance ()->UnregisterAllConnections ();
}

IConnection* ConnectionManager::CreateConnection (const char* name, const char* init_string)
{
  try
  {
    if (!init_string)
      init_string = "";

    ConnectionCreatorResult result;

    ConnectionManagerSingleton::Instance ()->GetCreator (name, result);

    return result.creator (name, init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::ConnectionManager::CreateConnection");
    throw;
  }
}
