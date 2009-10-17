#include "shared.h"

using namespace input::low_level::iphone_driver;

namespace
{

const char* COMPONENT_NAME = "input.low_level.iphone"; //имя компонента
const char* DRIVER_NAME    = "IPhone";                 //имя драйвера

const char* KEYBOARD_DEVICE_NAME = "Keyboard";

const char* SUPPORTED_DEVICES [] = { KEYBOARD_DEVICE_NAME };

}

namespace input
{

namespace low_level
{

namespace iphone_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///iPhone драйвер ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver ()
    {
    }

    ~Driver ()
    {
      DriverManager::UnregisterDriver (GetDescription ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "input::low_level::iphone_driver::Driver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных устройств ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetDevicesCount () 
    { 
      return sizeof (SUPPORTED_DEVICES) / sizeof (SUPPORTED_DEVICES [0]);
    }

    const char* GetDeviceName (size_t index)
    {
      if (index >= GetDevicesCount ())
        throw xtl::make_range_exception ("input::low_level::iphone_driver::Driver::GetDeviceName", "index", index, 0u, GetDevicesCount ());

      return SUPPORTED_DEVICES [index];
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создаение устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string = "") 
    {
      static const char* METHOD_NAME = "input::low_level::iphone_driver::Driver::CreateDevice";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!init_string)
        init_string = "";

      for (size_t i = 0, count = GetDevicesCount (); i < count; i++)
      {
        const char* device_name = GetDeviceName (i);

        if (!strcmp (device_name, name))
        {
          if (!strcmp (device_name, KEYBOARD_DEVICE_NAME))
            return new IPhoneKeyboard ();
        }
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

  private:
    Driver (const Driver& source);              //no impl
    Driver& operator = (const Driver& source);  //no impl

  private:
    LogHandler log_fn;
};

}

}

}

/*
    Компонент драйвера
*/

namespace
{

class IPhoneDriverComponent
{
  public:
    IPhoneDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<IPhoneDriverComponent> IPhoneInputDriver (COMPONENT_NAME);

}
