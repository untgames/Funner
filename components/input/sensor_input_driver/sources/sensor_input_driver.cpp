#include "shared.h"

using namespace common;
using namespace syslib;
using namespace input::low_level;
using namespace input::low_level::sensor_input_driver;

namespace
{

const char* COMPONENT_NAME = "input.low_level.SensorInputDriver"; //имя компонента
const char* DRIVER_NAME    = "SensorInputDriver";                 //имя драйвера

void default_log_handler (const char*)
{
}

}

/*
    Конструктор / деструктор
*/

Driver::Driver ()
  : debug_log (&default_log_handler)
{
  try
  {
    size_t sensors_count = SensorManager::SensorsCount ();
    
    devices_full_names.Reserve (sensors_count);
    
    for (size_t i=0; i<sensors_count; i++)
    {
      Sensor s (i);
      
      devices_full_names.Add (common::format ("%s.%s", s.Type (), s.Name ()).c_str ());
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Driver::Driver");
    throw;
  }
}

Driver::~Driver ()
{
}

/*
    Описание драйвера
*/

const char* Driver::GetDescription ()
{
  return "input::low_level::sensor_input_driver";
}

/*
    Перечисление доступных устройств ввода
*/

size_t Driver::GetDevicesCount ()
{
  return SensorManager::SensorsCount ();
}

const char* Driver::GetDeviceName (size_t index)
{
  try
  {
    return Sensor (index).Name ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Driver::GetDeviceName");
    throw;
  }
}

const char* Driver::GetDeviceFullName (size_t index)
{
  try
  {
    return devices_full_names [index];
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Driver::GetDeviceFullName");
    throw;
  }
}

/*
    Создаение устройства ввода
*/

IDevice* Driver::CreateDevice (const char* full_name, const char*)
{
  try
  {
    if (!full_name)
      throw xtl::make_null_argument_exception ("", "full_name");
    
    for (size_t i=0, count=devices_full_names.Size (); i<count; i++)
      if (!xtl::xstrcmp (devices_full_names [i], full_name))    
        return new Device (i, full_name);
        
    throw xtl::format_operation_exception ("", "Sensor '%s' not found", full_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Driver::CreateDevice");
    throw;
  }
}

/*
    Отладочное протоколирование
*/

void Driver::SetDebugLog (const LogHandler& log)
{
  debug_log = log;
}

const Driver::LogHandler& Driver::GetDebugLog ()
{
  return debug_log;
}

/*
    Подсчёт ссылок
*/

void Driver::AddRef ()
{
  addref (this);
}

void Driver::Release ()
{
  release (this);
}

/*
    Компонент драйвера
*/

namespace components
{

namespace sensor_input_driver
{

class SensorInputDriverComponent
{
  public:
    SensorInputDriverComponent ()
    {
      input::low_level::DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

extern "C"
{

common::ComponentRegistrator<SensorInputDriverComponent> SensorInputDriver (COMPONENT_NAME);

}

}

}
