#ifndef INPUT_LOW_LEVEL_TESTS_SHARED_HEADER
#define INPUT_LOW_LEVEL_TESTS_SHARED_HEADER

#include <stdio.h>
#include <string.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/common_exceptions.h>

#include <input/low_level/driver.h>
#include <input/low_level/device.h>

using namespace input::low_level;

const char* DEVICE_PROPERTY1 = "Axis_X_dead_zone";
const char* DEVICE_PROPERTY2 = "Axis_X_saturation_zone";

//тестовое устройство ввода
class TestInput: virtual public IDevice, public xtl::reference_counter
{
  public:
    TestInput () : dead_zone (0), saturation_zone (0) {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName () { return "TestInput"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Полное имя устройства (тип.имя.идентификатор)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetFullName () { return "type.test_input.1c4a1-9f2-21-aa0c"; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetEventHandler (const EventHandler& handler)
    {
      event_handler = handler;
    }
    const EventHandler& GetEventHandler ()
    {
      return event_handler;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProperties () {return "Axis_X_dead_zone Axis_X_saturation_zone";}
    void        SetProperty   (const char* name, float value) 
    {
      if (!strcmp (DEVICE_PROPERTY1, name))
        dead_zone = value;
      else if (!strcmp (DEVICE_PROPERTY2, name))
        saturation_zone = value;
      else
        throw xtl::make_argument_exception ("TestInput::SetProperty", "name", name);    
    }
    float GetProperty   (const char* name)
    {
      if (!strcmp (DEVICE_PROPERTY1, name))
        return dead_zone;
      else if (!strcmp (DEVICE_PROPERTY2, name))
        return saturation_zone;
      else
        throw xtl::make_argument_exception ("TestInput::GetProperty", "name", name);

      return 0;
    }    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef () { addref (this); }  
    void Release () { release (this); }

  private:
    float        dead_zone;
    float        saturation_zone;
    EventHandler event_handler;
};

//тестовый драйвер
class TestDriver: virtual public IDriver, public xtl::reference_counter
{
  public:
    TestDriver () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "TestDriver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных устройств ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetDevicesCount ()
    {
      throw xtl::make_not_implemented_exception ("TestDriver::GetDevicesCount");
      return 0;
    }
    const char* GetDeviceName (size_t index)
    {
      throw xtl::make_not_implemented_exception ("TestDriver::GetDeviceName");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создаение устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* name, const char* init_string = "") 
    {
      throw xtl::make_not_implemented_exception ("TestDriver::CreateDevice");
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

  private:
    LogHandler log_fn;
};

#endif
