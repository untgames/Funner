#include <xtl/any.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/strlib.h>

#include <input/low_level/driver.h>
#include <input/low_level/window_driver.h>

#include <client/window_manager.h>

using namespace client;
using namespace common;

namespace
{

/*
   Подсистема direct input драйвера               
*/

class WindowInputDriverSubsystem : public IEngineSubsystem, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    WindowInputDriverSubsystem (syslib::Window& window)
    {
      static size_t driver_index = 0;

      subsystem_name = format ("WindowInputSubsystem%u", driver_index);

      input::low_level::WindowDriver::RegisterDevice (subsystem_name.c_str (), window);

      device = DevicePtr (input::low_level::WindowDriver::Driver ()->CreateDevice (subsystem_name.c_str ()), false);

      driver_index++;
    }
    
/// Получение имени
    const char* Name () { return subsystem_name.c_str (); }

/// Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    WindowInputDriverSubsystem (const WindowInputDriverSubsystem&);             //no impl
    WindowInputDriverSubsystem& operator = (const WindowInputDriverSubsystem&); //no impl

  private:
    typedef xtl::com_ptr<input::low_level::IDevice> DevicePtr;

  private:
    stl::string subsystem_name;
    DevicePtr   device;
};

/*
   Создание драйвера
*/

void window_input_driver_startup (syslib::Window& window, common::VarRegistry& var_registry, Engine& engine)
{
  try
  {
    if (!var_registry.HasVariable ("Load"))
      throw xtl::format_operation_exception ("", "There is no 'Load' variable in the configuration registry branch '%s'", var_registry.BranchName ());

    if (!atoi (var_registry.GetValue ("Load").cast<stl::string> ().c_str ()))
      return;

    xtl::com_ptr<IEngineSubsystem> new_subsystem (new WindowInputDriverSubsystem (window), false);

    engine.AddSubsystem (new_subsystem.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("window_renderer_startup");
    throw;
  }
}

/*
   Компонент оконного ввода
*/

class WindowInputDriverComponent
{
  public:
    //загрузка компонента
    WindowInputDriverComponent () 
    {
      WindowManager::RegisterStartupHandler ("WindowInputDriver", &window_input_driver_startup);
    }
};

extern "C"
{

ComponentRegistrator<WindowInputDriverComponent> WindowInputDriver ("client.subsystems.window_input_driver");

}

}
