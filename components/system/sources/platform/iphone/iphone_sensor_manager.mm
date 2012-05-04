#include "shared.h"

using namespace syslib;


@interface AccelerometerDelegate : NSObject <UIAccelerometerDelegate>
{
  @private
    ISensorEventListener* listener;
}

- (id)initWithListener:(ISensorEventListener*)listener;

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

- (id)initWithListener:(ISensorEventListener*)in_listener
{
  self = [super init];

  if (!self)
    return nil;

  listener = in_listener;

  if (listener)
    [self becomeAccelerometerDelegate];

  return self;
}

- (void) dealloc
{
  [self resignAccelerometerDelegate];

  [super dealloc];
}

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
  SensorEvent event;

  memset (&event, 0, sizeof (event));

  event.timestamp    = acceleration.timestamp;
  event.values_count = 3;

  event.acceleration [2] = acceleration.z;

  switch ([UIApplication sharedApplication].statusBarOrientation)
  {
    case UIInterfaceOrientationPortraitUpsideDown:
      event.acceleration [0] = -acceleration.x;
      event.acceleration [1] = -acceleration.y;
      break;
    case UIInterfaceOrientationLandscapeLeft:
      event.acceleration [0] = acceleration.y;
      event.acceleration [1] = -acceleration.x;
      break;
    case UIInterfaceOrientationLandscapeRight:
      event.acceleration [0] = -acceleration.y;
      event.acceleration [1] = acceleration.x;
      break;
    default:
      event.acceleration [0] = acceleration.x;
      event.acceleration [1] = acceleration.y;
  }

  listener->OnSensorChanged (event);
}

@end

namespace
{

//Типы сенсоров
enum SensorType
{
  SensorType_Accelerometer,

  SensorType_Num
};

//Интерфейс сенсора
class ISensor : public xtl::reference_counter
{
  public:
    //Получение типа
    virtual const char* GetType () = 0;

    //Частота обновления
    virtual void  SetUpdateRate (float rate) = 0;
    virtual float GetUpdateRate () = 0;

    //Получение платформо-зависимого дескриптора экрана
    virtual const void* GetNativeHandle () = 0;

    //Чтение событий сенсора
    virtual void StartPolling (ISensorEventListener& listener) = 0;
    virtual void StopPolling  () = 0;

    //Получение имени
    virtual const char* GetName ()
    {
      return GetType ();
    }

    //Получение производителя
    virtual const char* GetVendor ()
    {
      return "Apple";
    }

    //Получение максимального значения
    virtual float GetMaxRange ()
    {
      return FLT_MAX;
    }

    virtual ~ISensor () {}
};

typedef xtl::intrusive_ptr<ISensor> SensorPtr;

class AccelerometerSensor : public ISensor
{
  public:
    //Конструктор / деструктор
    AccelerometerSensor ()
      : accelerometer_delegate (0)
      {}

    ~AccelerometerSensor ()
    {
      StopPolling ();
    }

    //Получение типа
    const char* GetType ()
    {
      return "Accelerometer";
    }

    //Частота обновления
    void  SetUpdateRate (float rate)
    {
      [UIAccelerometer sharedAccelerometer].updateInterval = 1 / rate;
    }

    float GetUpdateRate ()
    {
      return 1.f / [UIAccelerometer sharedAccelerometer].updateInterval;
    }

    //Получение платформо-зависимого дескриптора экрана
    const void* GetNativeHandle ()
    {
      return [UIAccelerometer sharedAccelerometer];
    }

    //Чтение событий сенсора
    void StartPolling (ISensorEventListener& listener)
    {
      StopPolling ();

      accelerometer_delegate = [[AccelerometerDelegate alloc] initWithListener:&listener];
    }

    void StopPolling ()
    {
      [accelerometer_delegate release];
      accelerometer_delegate = nil;
    }

  private:
    AccelerometerDelegate *accelerometer_delegate; //слушатель событий
};

}

namespace syslib
{

/// Дескриптор сенсора
struct sensor_handle
{
  SensorType type;      //тип сенсора
  SensorPtr  sensor;    //сенсор
  bool       polling;   //ведется ли в данный момент опрос сенсора

  sensor_handle (SensorType in_type)
    : type (in_type), polling (false)
  {
    switch (type)
    {
      case SensorType_Accelerometer:
        sensor = SensorPtr (new AccelerometerSensor (), false);
        break;
      default:
        throw xtl::make_argument_exception ("syslib::sensor_handle::sensor_handle", "type", type);
    }
  }
};

}

/*
    Количество сенсоров
*/

size_t IPhoneSensorManager::GetSensorsCount ()
{
  return SensorType_Num;
}

/*
    Создание / удаление сенсора
*/

sensor_t IPhoneSensorManager::CreateSensor (size_t sensor_index)
{
  //Защита от двойного создания сенсоров одного типа сделана снаружи
  if (sensor_index >= GetSensorsCount ())
    throw xtl::make_range_exception ("syslib::IPhoneSensorManager::CreateSensor", "sensor_index", sensor_index, 0u, GetSensorsCount ());

  return new sensor_handle ((SensorType)sensor_index);
}

void IPhoneSensorManager::DestroySensor (sensor_t handle)
{
  delete handle;
}

/*
    Имя сенсора
*/

stl::string IPhoneSensorManager::GetSensorName (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetSensorName", "handle");

  return handle->sensor->GetName ();
}

/*
    Производитель сенсора
*/

stl::string IPhoneSensorManager::GetSensorVendor (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetSensorVendor", "handle");

  return handle->sensor->GetVendor ();
}

/*
    Тип устройства
*/

stl::string IPhoneSensorManager::GetSensorType (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetSensorType", "handle");

  return handle->sensor->GetType ();
}

/*
    Максимальное значение
*/

float IPhoneSensorManager::GetSensorMaxRange (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetSensorMaxRange", "handle");

  return handle->sensor->GetMaxRange ();
}

/*
    Частота обновления
*/

void IPhoneSensorManager::SetSensorUpdateRate (sensor_t handle, float rate)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::SetSensorUpdateRate", "handle");

  handle->sensor->SetUpdateRate (rate);
}

float IPhoneSensorManager::GetSensorUpdateRate (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetSensorUpdateRate", "handle");

  return handle->sensor->GetUpdateRate ();
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* IPhoneSensorManager::GetNativeSensorHandle (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetNativeSensorHandle", "handle");

  return handle->sensor->GetNativeHandle ();
}

/*
    Получение платформо-зависимых свойств экрана
*/

void IPhoneSensorManager::GetSensorProperties (sensor_t handle, common::PropertyMap& properties)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::GetSensorProperties", "handle");
}

/*
    Чтение событий сенсора
*/

void IPhoneSensorManager::StartSensorPolling (sensor_t handle, ISensorEventListener& listener)
{
  static const char* METHOD_NAME = "syslib::IPhoneSensorManager::StartSensorPolling";

  if (!handle)
    throw xtl::make_null_argument_exception (METHOD_NAME, "handle");

  if (handle->polling)
    throw xtl::format_operation_exception (METHOD_NAME, "SensorEventListener has already registered");

  handle->sensor->StartPolling (listener);
  handle->polling = true;
}

void IPhoneSensorManager::StopSensorPolling (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::StopSensorPolling", "handle");

  handle->polling = false;
  handle->sensor->StopPolling ();
}

void IPhoneSensorManager::PollSensorEvents (sensor_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneSensorManager::PollSensorEvents", "handle");
}
