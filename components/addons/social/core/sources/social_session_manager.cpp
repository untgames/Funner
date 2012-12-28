#include "shared.h"

using namespace social;

/*
   Реализация менеджера сессий
*/

namespace
{

const char* COMPONENTS_MASK = "social.sessions.*"; //маска имён компонентов низкоуровневых драйверов отрисовки

struct SessionDesc
{
  stl::string                                  session_name_mask;
  social::SessionManager::CreateSessionHandler handler;

  SessionDesc (const char* in_mask, const social::SessionManager::CreateSessionHandler& in_handler)
    : session_name_mask (in_mask)
    , handler (in_handler)
    {}
};

}

struct SessionManagerImpl::Impl
{
  typedef stl::hash_map<stl::hash_key<const char*>, SessionDesc> SessionDescMap;

  SessionDescMap session_creators;

  ///Регистрация создателей сессий
  void RegisterSession (const char* id, const char* session_name_mask, const social::SessionManager::CreateSessionHandler& handler)
  {
    static const char* METHOD_NAME = "social::SessionManager::RegisterSession";

    if (!id)
      throw xtl::make_null_argument_exception (METHOD_NAME, "id");

    if (!session_name_mask)
      throw xtl::make_null_argument_exception (METHOD_NAME, "session_name_mask");

    SessionDescMap::iterator iter = session_creators.find (id);

    if (iter != session_creators.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Session with id '%s' already registered", id);

    session_creators.insert_pair (id, SessionDesc (session_name_mask, handler));
  }

  void UnregisterSession (const char* id)
  {
    if (!id)
      throw xtl::make_null_argument_exception ("social::SessionManager::UnregisterSession", "id");

    session_creators.erase (id);
  }

  void UnregisterAllSessions ()
  {
    session_creators.clear ();
  }

  ///Поиск сессии
  SessionDescMap::iterator FindSessionDesc (const char* session_name)
  {
    for (SessionDescMap::iterator iter = session_creators.begin (), end = session_creators.end (); iter != end; ++iter)
    {
      if (common::wcmatch (session_name, iter->second.session_name_mask.c_str ()))
        return iter;
    }

    return session_creators.end ();
  }

  ///Проверка наличия сессии
  bool IsSessionRegistered (const char* session_name)
  {
    if (!session_name)
      throw xtl::make_null_argument_exception ("social::SessionManager::IsSessionRegistered", "session_name");

    LoadDefaultSessions ();

    return FindSessionDesc (session_name) != session_creators.end ();
  }

  ///Создание сессии
  social::ISessionManager* CreateSession (const char* session_name)
  {
    static const char* METHOD_NAME = "social::SessionManagerImpl::CreateSession";

    if (!session_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "session_name");

    LoadDefaultSessions ();

    SessionDescMap::iterator iter = FindSessionDesc (session_name);

    if (iter == session_creators.end ())
      throw xtl::format_operation_exception (METHOD_NAME, "Session '%s' not registered", session_name);

    return iter->second.handler (session_name);
  }

  ///Загрузка сессий по умолчанию
  void LoadDefaultSessions ()
  {
    static common::ComponentLoader loader (COMPONENTS_MASK);
  }
};

/*
   Конструктор / деструктор
*/

SessionManagerImpl::SessionManagerImpl ()
  : impl (new Impl)
  {}

SessionManagerImpl::~SessionManagerImpl ()
{
  delete impl;
}

/*
   Регистрация создателей сессий
*/

void SessionManagerImpl::RegisterSession (const char* id, const char* session_name_mask, const social::SessionManager::CreateSessionHandler& handler)
{
  impl->RegisterSession (id, session_name_mask, handler);
}

void SessionManagerImpl::UnregisterSession (const char* id)
{
  impl->UnregisterSession (id);
}

void SessionManagerImpl::UnregisterAllSessions ()
{
  impl->UnregisterAllSessions ();
}

/*
   Проверка наличия сессии
*/

bool SessionManagerImpl::IsSessionRegistered (const char* session_name)
{
  return impl->IsSessionRegistered (session_name);
}

/*
   Создание сессии
*/

social::ISessionManager* SessionManagerImpl::CreateSession (const char* session_name)
{
  return impl->CreateSession (session_name);
}

/*
   Регистрация создателей сессий
*/

void SessionManager::RegisterSession (const char* id, const char* session_name_mask, const CreateSessionHandler& handler)
{
  SessionManagerSingleton::Instance ()->RegisterSession (id, session_name_mask, handler);
}

void SessionManager::UnregisterSession (const char* id)
{
  SessionManagerSingleton::Instance ()->UnregisterSession (id);
}

void SessionManager::UnregisterAllSessions ()
{
  SessionManagerSingleton::Instance ()->UnregisterAllSessions ();
}

/*
   Проверка наличия сессии
*/
    
bool SessionManager::IsSessionRegistered (const char* session_name)
{
  return SessionManagerSingleton::Instance ()->IsSessionRegistered (session_name);
}
