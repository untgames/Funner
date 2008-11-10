#include "shared.h"

using namespace tools::ui;
using namespace tools::ui::windows_forms;

/*
    Константы
*/

namespace
{

const char* PROFILE_NAME   = "WindowsForms";                    //имя профиля
const char* COMPONENT_NAME = "tools.ui.profiles.windows_forms"; //имя компонента

}

/*
    Компонент оконной системы
*/

namespace
{

class WindowsFormsComponent
{
  public:
    WindowsFormsComponent ()
    {
      WindowSystemManager::RegisterWindowSystem (PROFILE_NAME, &CreateWindowSystem);
    }
    
  private:
    static ICustomWindowSystem* CreateWindowSystem (const char* profile, IApplicationServer* server)
    {
      return new WindowSystem (server);
    }
};

}

extern "C"
{

common::ComponentRegistrator<WindowsFormsComponent> WindowsFormsSystem (COMPONENT_NAME);

}
