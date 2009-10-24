#include "shared.h"

#import <UIAccelerometer.h>

using namespace input::low_level::iphone_driver;

namespace
{

const double DEFAULT_UPDATE_INTERVAL = 1.f / 30.f;
const float  MAXIMUM_UPDATE_INTERVAL = 1000.f;
const size_t MESSAGE_BUFFER_SIZE     = 64;

const char* UPDATE_INTERVAL_PROPERTY = "UpdateInterval";

const char* PROPERTIES [] = {
  UPDATE_INTERVAL_PROPERTY
};

const size_t PROPERTIES_COUNT = sizeof (PROPERTIES) / sizeof (*PROPERTIES);

class AccelerometerListener
{
  public:
    virtual void OnAcceleration (double x, double y, double z) = 0;

  protected:
    virtual ~AccelerometerListener () {}
};

}

@interface AccelerometerDelegate : NSObject <UIAccelerometerDelegate>
{
  @private
    AccelerometerListener* listener;
    double                 update_interval;
}

@property AccelerometerListener* listener;
@property double                 update_interval;

- (id)initWithListener:(AccelerometerListener*)listener;

@end

@implementation AccelerometerDelegate

- (void)becomeAccelerometerDelegate
{
  [UIAccelerometer sharedAccelerometer].delegate = self;
}

- (void)resignAccelerometerDelegate
{
  if ([UIAccelerometer sharedAccelerometer].delegate == self)
    [UIAccelerometer sharedAccelerometer].delegate = nil;
}

- (id)init
{
  return [self initWithListener:0];
}

- (id)initWithListener:(AccelerometerListener*)in_listener
{
  self = [super init];

  if (!self)
    return nil;

  listener = in_listener;

  if (listener)
    [self becomeAccelerometerDelegate];

  self.update_interval = DEFAULT_UPDATE_INTERVAL;

  return self;
}

- (void) dealloc
{
  [self resignAccelerometerDelegate];

  [super dealloc];
}

- (double) update_interval
{
  return [UIAccelerometer sharedAccelerometer].updateInterval;
}

- (void) setUpdate_interval:(double)seconds
{
  [UIAccelerometer sharedAccelerometer].updateInterval = seconds;

  update_interval = seconds;

  if (update_interval > MAXIMUM_UPDATE_INTERVAL)
    [self resignAccelerometerDelegate];
}

- (AccelerometerListener*)listener
{
  return listener;
}

- (void)setListener:(AccelerometerListener*)in_listener
{
  listener = in_listener;

  if (!listener)
    [self resignAccelerometerDelegate];
  else if (update_interval < MAXIMUM_UPDATE_INTERVAL)
    [self becomeAccelerometerDelegate];
}

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
  if (!listener)
    return;

  listener->OnAcceleration (acceleration.x, acceleration.y, acceleration.z);
}

@end

struct IPhoneAccelerometer::Impl : public AccelerometerListener
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string            name;                   //имя устройства
  stl::string            full_name;              //полное имя устройства
  stl::string            properties;             //настройки
  DeviceSignal           signals;                //обработчики событий
  stl::wstring           control_name;           //имя контрола
  AccelerometerDelegate* accelerometer_delegate; //делегат системного акселерометра

  ///Конструктор / деструктор
  Impl (const char* in_name, const char* in_full_name)
    : name (in_name), full_name (in_full_name), accelerometer_delegate (0)
  {
    try
    {
      for (size_t i = 0; i < PROPERTIES_COUNT; i++)
      {
        properties.append (PROPERTIES[i]);
        properties.append (" ");
      }

      if (!properties.empty ())
        properties.pop_back ();

      accelerometer_delegate = [[AccelerometerDelegate alloc] initWithListener:this];
    }
    catch (xtl::exception& e)
    {
      [accelerometer_delegate release];
      e.touch ("input::low_level::iphone_driver::IPhoneAccelerometer::IPhoneAccelerometer");
      throw;
    }
  }

  ~Impl ()
  {
    [accelerometer_delegate release];
  }

  void OnAcceleration (double x, double y, double z)
  {
    static char message[MESSAGE_BUFFER_SIZE];

    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "Acceleration %f %f %f", x, y, z);
    signals (message);
  }
};

/*
   Конструктор/деструктор
*/

IPhoneAccelerometer::IPhoneAccelerometer (const char* name, const char* full_name)
  : impl (new Impl (name, full_name))
  {}

IPhoneAccelerometer::~IPhoneAccelerometer ()
{
  delete impl;
}

/*
   Получение имени устройства
*/

const char* IPhoneAccelerometer::GetName ()
{
  return impl->name.c_str ();
}

/*
   Полное имя устройства (тип.имя.идентификатор)
*/

const char* IPhoneAccelerometer::GetFullName ()
{
  return impl->full_name.c_str ();
}

/*
   Получение имени контрола
*/

const wchar_t* IPhoneAccelerometer::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   Подписка на события устройства
*/

xtl::connection IPhoneAccelerometer::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   Настройки устройства
*/

const char* IPhoneAccelerometer::GetProperties ()
{
  return impl->properties.c_str ();
}

void IPhoneAccelerometer::SetProperty (const char* name, float value)
{
  static const char* METHOD_NAME = "input::low_level::iphone_driver::IPhoneAccelerometer::SetProperty";

  if (!xtl::xstrcmp (UPDATE_INTERVAL_PROPERTY, name))
  {
    if (value <= 0)
      throw xtl::make_argument_exception (METHOD_NAME, "value", value, "Accelerometer update interval must be greater then 0");

    impl->accelerometer_delegate.update_interval = value;
  }
  else
    throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

float IPhoneAccelerometer::GetProperty (const char* name)
{
  if (!xtl::xstrcmp (UPDATE_INTERVAL_PROPERTY, name))
    return impl->accelerometer_delegate.update_interval;
  else
    throw xtl::make_argument_exception ("input::low_level::iphone_driver::IPhoneAccelerometer::GetProperty", "name", name);
}
