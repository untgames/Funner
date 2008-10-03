#include <xtl/function.h>
#include <xtl/common_exceptions.h>

#include <common/component.h>

#include <client/engine.h>
#include <client/window_manager.h>

using namespace client;
using namespace common;

namespace
{

/*
   Создание рендерера
*/

void window_manager_startup (common::VarRegistry& var_registry, IEngineStartupParams*, Engine& engine)
{
  try
  {
    WindowManager::InitWindows (var_registry, engine);
  }
  catch (xtl::exception& e)
  {
    e.touch ("window_manager_startup");
    throw;
  }
}

/*
   Компонент регистрации менеджера окон
*/

class WindowManagerComponent
{
  public:
    //загрузка компонента
    WindowManagerComponent () 
    {
      StartupManager::RegisterStartupHandler ("WindowManager", &window_manager_startup, StartupGroup_Level1);
    }
};

extern "C"
{

ComponentRegistrator<WindowManagerComponent> WindowManager ("client.subsystems.window_manager");

}

}
