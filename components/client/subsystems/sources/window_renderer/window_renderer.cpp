#include <xtl/any.h>
#include <xtl/function.h>
#include <xtl/common_exceptions.h>

#include <common/component.h>

#include <render/mid_level/window_driver.h>

#include <client/window_manager.h>

using namespace client;
using namespace common;

namespace
{

void test_registry_variable (VarRegistry& var_registry, const char* variable_name)
{
  if (!var_registry.HasVariable (variable_name))
    throw xtl::format_operation_exception ("", "There is no '%s' variable in the configuration registry branch '%s'",
      variable_name, var_registry.BranchName ());
}

/*
   Создание рендерера
*/


void window_renderer_startup (syslib::Window& window, common::VarRegistry& var_registry, Engine& engine)
{
  try
  {
    test_registry_variable (var_registry, "RendererName");

    render::mid_level::WindowDriver::RegisterWindow (var_registry.GetValue ("RendererName").cast<stl::string> ().c_str (), window, var_registry.BranchName ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("window_renderer_startup");
    throw;
  }
}

/*
   Компонент регистрации оконного рендерера
*/

class WindowRendererComponent
{
  public:
    //загрузка компонента
    WindowRendererComponent () 
    {
      WindowManager::RegisterStartupHandler ("WindowRenderer", &window_renderer_startup);
    }
};

extern "C"
{

ComponentRegistrator<WindowRendererComponent> WindowRenderer ("client.subsystems.window_renderer");

}

}
