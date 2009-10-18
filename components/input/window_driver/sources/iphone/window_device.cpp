#include <syslib/platform/iphone.h>

#include <shared/shared.h>

using namespace syslib::iphone;
using namespace input::low_level::window;

namespace
{

const size_t MESSAGE_BUFFER_SIZE = 64;
const double DEFAULT_ACCELEROMETER_UPDATE_INTERVAL = 1.f / 30.f;
const float  MAXIMUM_ACCELEROMETER_UPDATE_INTERVAL = 1000.f;
const float  EPSILON                               = 0.01f;

const char* TOUCH_NAME = "Touch";

const char* ACCELEROMETER_UPDATE_INTERVAL = "AccelerometerUpdateInterval";
const char* MULTITOUCH_ENABLED            = "MultitouchEnabled";

const char* PROPERTIES [] = {
  ACCELEROMETER_UPDATE_INTERVAL,
  MULTITOUCH_ENABLED
};

const size_t PROPERTIES_COUNT = sizeof (PROPERTIES) / sizeof (*PROPERTIES);

bool float_compare (float value1, float value2)
{
  return (value1 > (value2 - EPSILON)) && (value1 < (value2 + EPSILON));
}

}

struct Device::Impl : public IWindowListener, public IApplicationListener
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string                  name;                          //имя устройства
  stl::string                  full_name;                     //полное имя устройства
  stl::string                  properties;                    //настройки
  DeviceSignal                 signals;                       //обработчики событий
  stl::wstring                 control_name;                  //имя контрола
  xtl::auto_connection         window_destroy_connection;     //соединение удаления окна
  double                       accelerometer_update_interval; //частота прихода событий от акселерометра
  syslib::Window               *window;                       //окно
  char                         message [MESSAGE_BUFFER_SIZE]; //сообщение

  ///Конструктор / деструктор
  Impl (syslib::Window* in_window, const char* in_name, const char* in_full_name)
    : name (in_name)
    , full_name (in_full_name)
    , accelerometer_update_interval (DEFAULT_ACCELEROMETER_UPDATE_INTERVAL)
    , window (in_window)
  {
    try
    {
      window_destroy_connection = window->RegisterEventHandler (syslib::WindowEvent_OnDestroy, xtl::bind (&Device::Impl::OnWindowDestroy, this));

      for (size_t i = 0; i < PROPERTIES_COUNT; i++)
      {
        properties.append (PROPERTIES[i]);
        properties.append (" ");
      }

      if (!properties.empty ())
        properties.pop_back ();
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("input::low_level::window::Device::Device");
      throw;
    }

    attach_window_listener      (*window, this);
    attach_application_listener (this);

    set_accelerometer_update_interval (accelerometer_update_interval);
  }

  ~Impl ()
  {
    detach_application_listener (this);
    detach_window_listener      (*window, this);
  }

  ///События касания
  void OnTouchesBegan (size_t touches_count, const TouchDescription* touches)
  {
    for (size_t i = 0; i < touches_count; i++, touches++)
    {
      xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s began %p %f %f", TOUCH_NAME, touches->touch,
                 touches->x, touches->y);
      signals (message);

      if (!(touches->tap_count % 2))
      {
        xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s doubletap %p %f %f", TOUCH_NAME, touches->touch,
                   touches->x, touches->y);
        signals (message);
      }
    }
  }

  void OnTouchesMoved (size_t touches_count, const TouchDescription* touches)
  {
    for (size_t i = 0; i < touches_count; i++, touches++)
    {
      xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s moved %p %f %f", TOUCH_NAME, touches->touch,
                      touches->x, touches->y);
      signals (message);
    }
  }

  void OnTouchesEnded (size_t touches_count, const TouchDescription* touches)
  {
    for (size_t i = 0; i < touches_count; i++, touches++)
    {
      xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "%s ended %p %f %f", TOUCH_NAME, touches->touch,
                      touches->x, touches->y);
      signals (message);
    }
  }

  ///События движения
  void OnShakeMotionBegan ()
  {
    signals ("ShakeMotion began");
  }

  void OnShakeMotionEnded ()
  {
    signals ("ShakeMotion ended");
  }

  void OnWindowDestroy ()
  {
    detach_window_listener (*window, this);
  }

  ///События приложения
  void OnAcceleration (double x, double y, double z)
  {
    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "Acceleration %f %f %f", x, y, z);
    signals (message);
  }

  void OnMemoryWarning ()
  {
    signals ("MemoryWarning");
  }

  void OnActive ()
  {
    signals ("Application active");
  }

  void OnInactive ()
  {
    signals ("Application inactive");
  }
};

/*
   Конструктор/деструктор
*/

Device::Device (syslib::Window* window, const char* name, const char* full_name)
  : impl (new Impl (window, name, full_name))
{
}

Device::~Device ()
{
  delete impl;
}

/*
   Получение имени контрола
*/

const wchar_t* Device::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   Подписка на события устройства
*/

xtl::connection Device::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   Настройки устройства
*/

const char* Device::GetProperties ()
{
  return impl->properties.c_str ();
}

void Device::SetProperty (const char* name, float value)
{
  static const char* METHOD_NAME = "input::low_level::window::Device::SetProperty";

  if (!xtl::xstrcmp (ACCELEROMETER_UPDATE_INTERVAL, name))
  {
    detach_application_listener (impl);

    if (value < MAXIMUM_ACCELEROMETER_UPDATE_INTERVAL)
    {
      attach_application_listener (impl);

      impl->accelerometer_update_interval = value;

      set_accelerometer_update_interval (impl->accelerometer_update_interval);
    }
  }
  else if (!xtl::xstrcmp (MULTITOUCH_ENABLED, name))
  {
    if (float_compare (value, 0.f))
      set_multitouch_enabled (*impl->window, false);
    else if (float_compare (value, 1.f))
      set_multitouch_enabled (*impl->window, true);
    else
      throw xtl::format_operation_exception (METHOD_NAME, "Set only 0 or 1 for property '%s'", MULTITOUCH_ENABLED);
  }
  else
    throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

float Device::GetProperty (const char* name)
{
  if (!xtl::xstrcmp (ACCELEROMETER_UPDATE_INTERVAL, name))
    return impl->accelerometer_update_interval;
  else if (!xtl::xstrcmp (MULTITOUCH_ENABLED, name))
    return get_multitouch_enabled (*impl->window) ? 1.f : 0.f;
  else
    throw xtl::make_argument_exception ("input::low_level::window::Device::GetProperty", "name", name);
}

/*
   Получение имени устройства
*/

const char* Device::GetName ()
{
  return impl->name.c_str ();
}

/*
   Полное имя устройства (тип.имя.идентификатор)
*/

const char* Device::GetFullName ()
{
  return impl->full_name.c_str ();
}
