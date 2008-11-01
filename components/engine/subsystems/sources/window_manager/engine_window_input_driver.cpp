#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <input/low_level/driver.h>
#include <input/low_level/window_driver.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "WindowInputDriver";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.WindowInputDriver"; //имя компонента

/*
    Подсистема direct input драйвера               
*/

class WindowInputDriverSubsystem : public ISubsystem, public IAttachmentRegistryListener<syslib::Window>, public xtl::reference_counter
{
  public:
/// Конструктор
    WindowInputDriverSubsystem (ParseNode& node)
      : attachment_name (get<const char*> (node, "Window")),
        device_name (get<const char*> (node, "DeviceName", ""))
    {              
      if (device_name.empty ())
      {
        static size_t driver_index = 1;

        device_name = format ("WindowInputSubsystem%u", driver_index++);
      }      

      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
    }
    
///Деструктор
    ~WindowInputDriverSubsystem ()
    {
      AttachmentRegistry::Detach (this, AttachmentRegistryAttachMode_ForceNotify);
    }
    
///Обработчик события регистрации окна
    void OnRegisterAttachment (const char* name, syslib::Window& window)
    {
      if (name != attachment_name)
        return;
      
      input::low_level::WindowDriver::UnregisterDevice (device_name.c_str ());
      input::low_level::WindowDriver::RegisterDevice (device_name.c_str (), window);
    }
    
///Обработчик события отмены регистрации окна
    void OnUnregisterAttachment (const char* name, syslib::Window&)
    {
      if (name != attachment_name)
        return;

      input::low_level::WindowDriver::UnregisterDevice (device_name.c_str ());
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    WindowInputDriverSubsystem (const WindowInputDriverSubsystem&);             //no impl
    WindowInputDriverSubsystem& operator = (const WindowInputDriverSubsystem&); //no impl

  private:
    typedef xtl::com_ptr<input::low_level::IDevice> DevicePtr;

  private:
    stl::string attachment_name;
    stl::string device_name;
};

/*
    Компонент оконного ввода
*/

class WindowInputDriverComponent
{
  public:
    WindowInputDriverComponent () 
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ISubsystem> subsystem (new WindowInputDriverSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::WindowInputDriverComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<WindowInputDriverComponent> WindowInputDriver (COMPONENT_NAME);

}

}
