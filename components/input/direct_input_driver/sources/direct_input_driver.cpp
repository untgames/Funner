#include "shared.h"

using namespace common;

namespace
{

const char* COMPONENT_NAME = "input.low_level.direct_input8"; //имя компонента
const char* DRIVER_NAME    = "DirectInput8";                  //имя драйвера

void default_log_handler (const char*)
{
}

BOOL FAR PASCAL enum_devices_callback (LPCDIDEVICEINSTANCE device_instance, LPVOID direct_input_driver);

const char* get_com_error_name (HRESULT error)
{
  switch (error)
  {
    case CLASS_E_NOAGGREGATION: return "CLASS_E_NOAGGREGATION";
    case E_INVALIDARG:          return "E_INVALIDARG";
    case E_NOINTERFACE:         return "E_NOINTERFACE";
    case E_OUTOFMEMORY:         return "E_OUTOFMEMORY";
    case E_UNEXPECTED:          return "E_UNEXPECTED";
    case REGDB_E_CLASSNOTREG:   return "REGDB_E_CLASSNOTREG";
    case RPC_E_CHANGED_MODE:    return "RPC_E_CHANGED_MODE";
    default:                    return "Unknown";
  }
}

enum DirectInputDeviceType
{
  DirectInputDeviceType_GameControl,
  DirectInputDeviceType_Keyboard,
  DirectInputDeviceType_Pointer,
  DirectInputDeviceType_Other
};

const char* get_device_type_name (DirectInputDeviceType type)
{
  switch (type)
  {
    case DirectInputDeviceType_GameControl: return "Joystick";
    case DirectInputDeviceType_Keyboard:    return "Keyboard";
    case DirectInputDeviceType_Pointer:     return "Pointer";
    case DirectInputDeviceType_Other:
    default:                                return "Unknown";
  }
}

struct ComInitializer
{
  ComInitializer ()
  {
    HRESULT create_result;

    create_result = CoInitialize (0);

    switch (create_result)
    {
      case S_OK:
      case S_FALSE:
        break;
      default:
        throw xtl::format_operation_exception ("ComInitializer::ComInitializer", "Can't initialize COM, error '%s'.", get_com_error_name (create_result));
    }
  }

  ~ComInitializer ()
  {
    CoUninitialize ();
  }
};

typedef common::Singleton<ComInitializer> ComInitializerSingleton;

}

namespace input
{

namespace low_level
{

namespace direct_input_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Window драйвер ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
    Driver () 
      : log_fn (&default_log_handler)
    {
      static const char* METHOD_NAME = "input::low_level::direct_input_driver::Driver::Driver";

      try
      {
        ComInitializerSingleton::Instance ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);
      }

      HRESULT create_result;

      create_result = CoCreateInstance (CLSID_DirectInput8, 0, CLSCTX_INPROC_SERVER, IID_IDirectInput8A, (LPVOID *)&direct_input_interface);

      if (create_result != S_OK)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't create direct input 8 interface, error '%s'", get_com_error_name (create_result));

      create_result = direct_input_interface->Initialize (GetModuleHandle (0), DIRECTINPUT_VERSION);

      if (create_result != DI_OK)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't initialize direct input interface, error '%s'", get_direct_input_error_name (create_result));

      RegisterDevices ();
    }

    ~Driver () 
    { 
      UnregisterDriver ();

      direct_input_interface->Release ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "input::low_level::direct_input_driver::Driver"; }

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
        throw xtl::make_range_exception ("input::low_level::direct_input_driver::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return device_entries[index]->device_name.c_str ();
    }

    const char* GetDeviceFullName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("input::low_level::direct_input_driver::Driver::GetDeviceFullName", "index", index, 0u, GetDevicesCount ());

      return device_entries[index]->device_full_name.c_str ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создаение устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string = "") 
    {
      static const char* METHOD_NAME = "input::low_level::direct_input_driver::Driver::CreateDevice";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!init_string)
        init_string = "";

      for (DeviceEntries::iterator iter = device_entries.begin (), end = device_entries.end (); iter != end; ++iter)
        if (!xtl::xstrcmp ((*iter)->device_name.c_str (), name))
        {
          HRESULT create_result;

          IDirectInputDevice8 *device_interface;

          create_result = direct_input_interface->CreateDevice ((*iter)->device_guid, &device_interface, NULL);

          if (create_result != DI_OK)
            throw xtl::format_operation_exception (METHOD_NAME, "Can't create direct input device, error '%s'", get_direct_input_error_name (create_result));

          return new OtherDevice (&dummy_window, name, (*iter)->device_full_name.c_str (), device_interface, log_fn, get_device_type_name ((*iter)->device_type), init_string);
        }

      throw xtl::make_argument_exception (METHOD_NAME, "name", name);
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
    void AddRef () {}  
    void Release () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация устройства ввода по имени устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterDevice (const char* device_name, REFGUID guid)
    {
      try
      {
        stl::string full_name = common::format ("%s.%s.%x-%x-%x-%x", get_device_type_name (current_device_type), device_name, ((size_t*)&guid)[0], ((size_t*)&guid)[1], ((size_t*)&guid)[2], ((size_t*)&guid)[3]);

        device_entries.push_back (DeviceEntryPtr (new DeviceEntry (device_name, full_name.c_str (), guid, current_device_type), false));
      }
      catch (...)
      {
        //подавление исключений
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация окна как устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterDevices ()
    {
      current_device_type = DirectInputDeviceType_GameControl;
      direct_input_interface->EnumDevices (DI8DEVCLASS_GAMECTRL, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      current_device_type = DirectInputDeviceType_Keyboard;
      direct_input_interface->EnumDevices (DI8DEVCLASS_KEYBOARD, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      current_device_type = DirectInputDeviceType_Pointer;
      direct_input_interface->EnumDevices (DI8DEVCLASS_POINTER, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);

      current_device_type = DirectInputDeviceType_Other;
      direct_input_interface->EnumDevices (DI8DEVCLASS_DEVICE, &enum_devices_callback, this, DIEDFL_ATTACHEDONLY);
    }

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl

  private:
    void UnregisterDriver ()
    {
      DriverManager::UnregisterDriver (GetDescription ());
    }

  private:
    struct DeviceEntry : public xtl::reference_counter
    {
      stl::string           device_name;
      stl::string           device_full_name;
      GUID                  device_guid;
      DirectInputDeviceType device_type;

      DeviceEntry (const char* in_device_name, const char* in_device_full_name, const GUID& in_device_guid, DirectInputDeviceType in_device_type)
        : device_name (in_device_name), device_full_name (in_device_full_name), device_guid (in_device_guid), device_type (in_device_type)
        {}
    };

    typedef xtl::intrusive_ptr<DeviceEntry>     DeviceEntryPtr;
    typedef stl::vector<DeviceEntryPtr>         DeviceEntries;

  private:
    DeviceEntries         device_entries;
    LogHandler            log_fn;
    IDirectInput8         *direct_input_interface;
    DirectInputDeviceType current_device_type;
    syslib::Window        dummy_window;
};

}

}

}

namespace
{

using input::low_level::direct_input_driver::Driver;

BOOL FAR PASCAL enum_devices_callback (LPCDIDEVICEINSTANCE device_instance, LPVOID direct_input_driver)
{
#ifdef DIRECT_INPUT_KEYBOARD_DISABLED
  if (device_instance->guidInstance != GUID_SysKeyboard)
#endif
    ((Driver*)direct_input_driver)->RegisterDevice (device_instance->tszInstanceName, device_instance->guidInstance);
  
  return DIENUM_CONTINUE;
}

}

/*
    Компонент драйвера
*/

namespace
{

class DirectInputDriverComponent
{
  public:
    DirectInputDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<DirectInputDriverComponent> DirectInput8Driver (COMPONENT_NAME);

}
