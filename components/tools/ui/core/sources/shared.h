#ifndef TOOLS_UI_CORE_SHARED_HEADER
#define TOOLS_UI_CORE_SHARED_HEADER

#include <stl/hash_map>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>

#include <common/component.h>
#include <common/file.h>
#include <common/singleton.h>

#include <tools/ui/custom_window_system.h>
#include <tools/ui/main_window.h>

namespace tools
{

namespace ui
{

typedef xtl::com_ptr<ICustomWindowSystem> CustomWindowSystemPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация менеджера оконных систем
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowSystemManagerImpl
{
  public:
    typedef WindowSystemManager::WindowSystemCreator WindowSystemCreator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация оконных систем
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterWindowSystem       (const char* profile, const WindowSystemCreator& creator);
    void UnregisterWindowSystem     (const char* profile);
    void UnregisterAllWindowSystems ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация серверов приложений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterApplicationServer       (const char* application_server_name, IApplicationServer* server);
    void UnregisterApplicationServer     (const char* application_server_name);
    void UnregisterAllApplicationServers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание оконной системы
///////////////////////////////////////////////////////////////////////////////////////////////////
    CustomWindowSystemPtr CreateWindowSystem (const char* application_server_name, const char* profile);

  private:
    typedef xtl::com_ptr<IApplicationServer>                                ApplicationServerPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, ApplicationServerPtr> ApplicationServerMap;
    typedef stl::hash_map<stl::hash_key<const char*>, WindowSystemCreator>  WindowSystemMap;

  private:
    ApplicationServerPtr GetApplicationServer (const char* server_name);

  private:
    ApplicationServerMap application_servers;
    WindowSystemMap      windows_systems;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон менеджера оконных систем
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<WindowSystemManagerImpl> WindowSystemManagerSingleton;

}

}

#endif

