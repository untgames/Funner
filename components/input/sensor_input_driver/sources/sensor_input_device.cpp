#include "shared.h"

using namespace common;
using namespace syslib;
using namespace input::low_level;
using namespace input::low_level::sensor_input_driver;

/*
    Константы
*/

namespace
{

const size_t EVENT_BUFFER_SIZE        = 512;
const char*  UPDATE_INTERVAL_PROPERTY = "UpdateInterval";

}

/*
    Конструктор / деструктор
*/

Device::Device (size_t sensor_index, const char* in_full_name)
  : sensor (sensor_index)
{
  full_name = in_full_name;
  
  event_buffer.resize (EVENT_BUFFER_SIZE);
  
  type_offset = xtl::xsnprintf (&event_buffer [0], event_buffer.size (), "%s", sensor.Type ());  
  properties  = UPDATE_INTERVAL_PROPERTY;
  
  connect_tracker (sensor.RegisterEventHandler (xtl::bind (&Device::Notify, this, _1)));
}

Device::~Device ()
{
}

/*
    Имя устройства
*/

const char* Device::GetName ()
{
  return sensor.Name ();
}

/*
    Полное имя устройства (тип.имя.идентификатор)
*/

const char* Device::GetFullName ()
{
  return full_name.c_str ();
}

/*
    Получение имени контрола
*/

const wchar_t* Device::GetControlName (const char* control_id)
{
  control_name = common::towstring (control_id);

  return control_name.c_str ();
}

/*
    Подписка на события устройства
*/

void Device::Notify (const syslib::SensorEvent& event)
{
  char* s = &event_buffer [0] + type_offset + 1, *end = &event_buffer [0] + event_buffer.size ();
  
  size_t value_index = 0;

  while (s<end && value_index < event.values_count)
  {
    if (s <= end)
      s [-1] = ' ';
    
    size_t length = xtl::xsnprintf (s, end - s, "%.3f", event.data [value_index]);    
    
    s += length + 1;
    value_index++;    
  }

  signal (event_buffer.c_str ());
}

xtl::connection Device::RegisterEventHandler (const EventHandler& handler)
{
  return signal.connect (handler);
}

/*
    Настройки устройства
*/

const char* Device::GetProperties ()
{
  return properties.c_str ();
}

void Device::SetProperty (const char* name, float value)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    if (!xtl::xstrcmp (UPDATE_INTERVAL_PROPERTY, name))
    {
      if (value <= 0)
        throw xtl::make_argument_exception ("", "value", value, "Sensor update interval must be greater then 0");

      sensor.SetUpdateRate (1.0f / value);
    }

    throw xtl::make_argument_exception ("", "name", name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Device::SetProperty");
    throw;
  }
}

float Device::GetProperty (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");    
    
    if (!xtl::xstrcmp (UPDATE_INTERVAL_PROPERTY, name))
    {
      float rate = sensor.UpdateRate ();
      
      static float EPS = 0.001f;
      
      if (rate < EPS)
        return 0.0;
      
      return 1.0f / rate;
    }

    throw xtl::make_argument_exception ("", "name", name);        
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::sensor_input_driver::Device::GetProperty");
    throw;
  }
}

/*
    Подсчёт ссылок
*/

void Device::AddRef ()
{
  addref (this);
}

void Device::Release ()
{
  release (this);
}
