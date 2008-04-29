#include <stdio.h>

#include <common/exception.h>
#include <common/singleton.h>

#include <stl/vector>
#include <stl/string>

#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/connection.h>
#include <xtl/bind.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>
#include <input/low_level/window_driver.h>

#include <syslib/window.h>

#include "shared.h"

using namespace common;
using namespace stl;
using namespace syslib;

namespace
{

void default_log_handler (const char*)
{
}

}

namespace input
{

namespace low_level
{

namespace window
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Window драйвер ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
    Driver () : log_fn (&default_log_handler) {}

    ~Driver () 
    { 
      UnregisterDriver ();
      Release (); 
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "Window driver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных устройств ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetDevicesCount () 
    { 
      return device_entries.size (); 
    }

    const char* GetDeviceName (size_t index)
    {
      if (index >= GetDevicesCount ())
        RaiseOutOfRange ("input::low_level::window::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return device_entries[index]->device_name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создаение устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name) 
    {
      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!strcmp ((*iter)->device_name.c_str (), name))
          return new Device ((*iter)->window, name);

      RaiseInvalidArgument ("input::low_level::window::Driver::CreateDevice", "name", name);
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка функции отладочного протоколирования драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////   
    void SetDebugLog (const LogHandler& in_log)
    {
      log_fn = in_log;
    }
    
    const LogHandler& GetDebugLog ()
    {
      return log_fn;
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация окна как устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterDevice (const char* device_name, Window& window)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!strcmp ((*iter)->device_name.c_str (), device_name))
          RaiseInvalidArgument ("input::low_level::window::Driver::RegisterDevice", "device_name", device_name, "Name already registered");

      device_entries.push_back (DeviceEntryPtr (new DeviceEntry (device_name, &window, window.RegisterEventHandler (WindowEvent_OnDestroy, xtl::bind (&Driver::DestroyWindowHandler, this, _1, _2, _3)))));

      if (device_entries.size () == 1)
        DriverManager::RegisterDriver (GetDescription (), this);
    }

    void UnregisterDevice (const char* device_name)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!strcmp ((*iter)->device_name.c_str (), device_name))
        {
          device_entries.erase (iter);

          if (device_entries.empty ())
            UnregisterDriver ();

          return;
        }
    }

    void UnregisterAllDevices (Window& window)
    {
      for (DeviceEntries::iterator iter = device_entries.begin (); iter != device_entries.end (); ++iter)
        if ((*iter)->window == &window)
          device_entries.erase (iter--);

      if (device_entries.empty ())
        UnregisterDriver ();
    }

    void UnregisterAllDevices ()
    {
      device_entries.clear ();
      UnregisterDriver ();
    }

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl

  private:
    void UnregisterDriver ()
    {
      DriverManager::UnregisterDriver (GetDescription ());
    }

    void DestroyWindowHandler (Window& window, WindowEvent, const WindowEventContext&)
    {
      UnregisterAllDevices (window);
    }

  private:
    struct DeviceEntry
    {
      string               device_name;
      Window*              window;
      xtl::auto_connection on_window_destroy_connection;

      DeviceEntry (const char* in_device_name, Window* in_window, xtl::connection& in_connection) 
        : device_name (in_device_name), window (in_window), on_window_destroy_connection (in_connection)
        {}
    };

    typedef xtl::shared_ptr<DeviceEntry> DeviceEntryPtr;
    typedef vector<DeviceEntryPtr>       DeviceEntries;

  private:
    DeviceEntries   device_entries;
    LogHandler      log_fn;
};


}

}

}

namespace
{

typedef Singleton<input::low_level::window::Driver> WindowDriverSingleton;

}

namespace input
{

namespace low_level
{

/*
   Window-драйвер ввода
*/

/*
   Имя драйвера
*/

const char* WindowDriver::Name ()
{
  return WindowDriverSingleton::Instance ().GetDescription ();
}

/*
   Получение драйвера
*/

IDriver* WindowDriver::Driver ()
{
  return WindowDriverSingleton::InstancePtr ();
}

/*
   Регистрация окна как устройства ввода
*/

void WindowDriver::RegisterDevice (const char* device_name, syslib::Window& window)
{
  if (!device_name)
    RaiseNullArgument ("input::low_level::WindowDriver::RegisterDevice", "device_name");

  WindowDriverSingleton::Instance ().RegisterDevice (device_name, window);
}

void WindowDriver::RegisterDevice (syslib::Window& window)
{
  RegisterDevice (window.Title (), window);
}

void WindowDriver::UnregisterDevice (const char* device_name)
{
  if (!device_name)
    RaiseNullArgument ("input::low_level::WindowDriver::UnregisterDevice", "device_name");

  WindowDriverSingleton::Instance ().UnregisterDevice (device_name);
}

void WindowDriver::UnregisterAllDevices (syslib::Window& window)
{
  WindowDriverSingleton::Instance ().UnregisterAllDevices (window);
}

void WindowDriver::UnregisterAllDevices ()
{
  WindowDriverSingleton::Instance ().UnregisterAllDevices ();
}

}

}
