#include "shared.h"

using namespace tools::ui;

/*
    Регистрация оконных систем
*/

//регистрация оконной системы  
void WindowSystemManagerImpl::RegisterWindowSystem (const char* profile, const WindowSystemCreater& creater)
{
  static const char* METHOD_NAME = "tools::ui::WindowSystemManagerImpl::RegisterWindowSystem";
  
  if (!profile)
    throw xtl::make_null_argument_exception (METHOD_NAME, "profile");
    
  WindowSystemMap::iterator iter = windows_systems.find (profile);
  
  if (iter != windows_systems.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "profile", profile, "Profile already registered");
    
  windows_systems.insert_pair (profile, creater);      
}

//отмена регистрации оконной системы с указанным именем
void WindowSystemManagerImpl::UnregisterWindowSystem (const char* profile)
{
  if (!profile)
    return;
    
  windows_systems.erase (profile);
}

//отмена регистрации всех оконных систем
void WindowSystemManagerImpl::UnregisterAllWindowSystems ()
{
  windows_systems.clear ();
}        

/*
    Регистрация серверов приложений
*/

//регистрация сервера приложения
void WindowSystemManagerImpl::RegisterApplicationServer (const char* application_server_name, IApplicationServer* server)
{
  static const char* METHOD_NAME = "tools::ui::WindowSystemManagerImpl::RegisterApplicationServer";
  
  if (!application_server_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "application_server_name");

  if (!server)
    throw xtl::make_null_argument_exception (METHOD_NAME, "server");

  ApplicationServerMap::iterator iter = application_servers.find (application_server_name);

  if (iter != application_servers.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "application_server_name", application_server_name, "Application server already registered");

  application_servers.insert_pair (application_server_name, server);
}

//отмена регистрации сервера приложения
void WindowSystemManagerImpl::UnregisterApplicationServer (const char* application_server_name)
{
  if (!application_server_name)
    return;

  application_servers.erase (application_server_name);
}

//отмена регистрации всех серверов приложений
void WindowSystemManagerImpl::UnregisterAllApplicationServers ()
{
  application_servers.clear ();
}

/*
    Создание оконной системы
*/

//получение сервера приложения
WindowSystemManagerImpl::ApplicationServerPtr WindowSystemManagerImpl::GetApplicationServer (const char* server_name)
{
  static const char* METHOD_NAME = "tools::ui::WindowSystemManagerImpl::GetApplicationServer";

  if (!server_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "server_name");

  ApplicationServerMap::iterator iter = application_servers.find (server_name);

  if (iter == application_servers.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "server_name", server_name, "No application server with this name");

  return iter->second;
}

//создание оконной системы
CustomWindowSystemPtr WindowSystemManagerImpl::CreateWindowSystem (const char* application_server_name, const char* profile)
{
  try
  {
      //получение сервера приложения

    ApplicationServerPtr application = GetApplicationServer (application_server_name);
    
      //поиск порождающей функции оконной системы

    if (!profile)
      throw xtl::make_null_argument_exception ("", "profile");

    WindowSystemMap::iterator iter = windows_systems.find (profile);
    
    if (iter == windows_systems.end ())
      throw xtl::make_argument_exception ("", "profile", profile, "No window system profile with this name");

      //создание оконной системы

    return CustomWindowSystemPtr (iter->second (profile, application.get ()), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("tools::ui::WindowSystemManagerImpl::CreateWindowSystem");
    throw;
  }
}

/*
    WindowSystemManager
*/

void WindowSystemManager::RegisterWindowSystem (const char* profile, const WindowSystemCreater& creater)
{
  WindowSystemManagerSingleton::Instance ().RegisterWindowSystem (profile, creater);
}

void WindowSystemManager::UnregisterWindowSystem (const char* profile)
{
  WindowSystemManagerSingleton::Instance ().UnregisterWindowSystem (profile);
}

void WindowSystemManager::UnregisterAllWindowSystems ()
{
  WindowSystemManagerSingleton::Instance ().UnregisterAllWindowSystems ();
}

void WindowSystemManager::RegisterApplicationServer (const char* application_server_name, IApplicationServer* server)
{
  WindowSystemManagerSingleton::Instance ().RegisterApplicationServer (application_server_name, server);
}

void WindowSystemManager::UnregisterApplicationServer (const char* application_server_name)
{
  WindowSystemManagerSingleton::Instance ().UnregisterApplicationServer (application_server_name);
}

void WindowSystemManager::UnregisterAllApplicationServers ()
{
  WindowSystemManagerSingleton::Instance ().UnregisterAllApplicationServers ();
}
