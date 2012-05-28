#include "shared.h"

namespace syslib
{

class TabletOsSensorHandler
{
  friend class TabletOsSensorManagerImpl;
  friend class TabletOsSensorManager;

  public:
    TabletOsSensorHandler (sensor_type_t t):
      type (t),
      rate (0)
    {
      try
      {
        sensor_info (type, &info);
//sensor_info возвращает ошибку, но данные потом удут корректные ?????????
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensorHandler::TabletOsSensorHandler");
        throw;
      }
    }

    ~TabletOsSensorHandler ()
    {
      try
      {
        if (sensor_info_destroy (info) == BPS_FAILURE);
          tabletos::raise_error ("::sensor_info_destroy");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensorHandler::~TabletOsSensorHandler");
        throw;
      }
    }

    sensor_type_t GetType ()
    {
      return type;
    }

    float MaxRange ()
    {
      return sensor_info_get_range_maximum (info);
    }

    void SetRate (unsigned int r)
    {
      try
      {
        if (sensor_set_rate (type, r) == BPS_FAILURE);
          tabletos::raise_error ("::sensor_set_rate");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensorHandler::SetRate");
        throw;
      }
    }
 
    unsigned int GetRate ()
    {
      return rate;
    }

    void GetProperties (common::PropertyMap& properties)
    {
      unsigned int int_val;
      float float_val;
     
      float_val = sensor_info_get_resolution (info);
      properties.SetProperty ("Resolution", float_val);

      float_val = sensor_info_get_range_minimum (info);
      properties.SetProperty ("Minimum Range", float_val);

      int_val = sensor_info_get_delay_mininum (info);
      properties.SetProperty ("Minimum Delay", (int)int_val);

      int_val = sensor_info_get_delay_maximum (info);
      properties.SetProperty ("Maximum Delay", (int)int_val);

      int_val = sensor_info_get_delay_default (info);
      properties.SetProperty ("Default Delay", (int)int_val);

      float_val = sensor_info_get_power (info);
      properties.SetProperty ("Power", float_val);
    }

  private:
    sensor_type_t type;
    sensor_info_t *info;
    unsigned int rate;
};

class TabletOsSensorManagerImpl
{
  public:
    TabletOsSensorManagerImpl ()
    {
      sensor_type_t sens[] = {SENSOR_TYPE_ACCELEROMETER,SENSOR_TYPE_MAGNETOMETER,SENSOR_TYPE_GYROSCOPE,
        SENSOR_TYPE_AZIMUTH_PITCH_ROLL,SENSOR_TYPE_ALTIMETER,SENSOR_TYPE_TEMPERATURE,SENSOR_TYPE_PROXIMITY,
        SENSOR_TYPE_LIGHT,SENSOR_TYPE_GRAVITY,SENSOR_TYPE_LINEAR_ACCEL,SENSOR_TYPE_ROTATION_VECTOR,
        SENSOR_TYPE_ROTATION_MATRIX};

      for (unsigned int i=0; i<sizeof (sens) / sizeof (sensor_type_t); i++)
        if (sensor_is_supported (sens [i]))
          sensors.push_back (sens [i]);
    }

    size_t GetSensorsCount ()
    {
      return sensors.size ();
    }

    TabletOsSensorHandler *CreateHandler (size_t index)
    {
      TabletOsSensorHandler *handler = new TabletOsSensorHandler (sensors [index]);
      return handler;
    } 

    void DestroyHandler (TabletOsSensorHandler *handler)
    {
      delete handler;
    } 

  private:
    stl::vector<sensor_type_t> sensors;
};

typedef common::Singleton<TabletOsSensorManagerImpl> TabletOsSensorManagerImplSingleton;


/*
    Количество сенсоров
*/

size_t TabletOsSensorManager::GetSensorsCount ()
{
  return TabletOsSensorManagerImplSingleton::Instance ()->GetSensorsCount ();
}

/*
    Создание / удаление сенсора
*/

sensor_t TabletOsSensorManager::CreateSensor (size_t sensor_index)
{
  try
  {
    TabletOsSensorManagerImplSingleton::Instance manager;
    
    if (sensor_index >= manager->GetSensorsCount ())
      throw xtl::make_range_exception ("", "sensor_index", sensor_index, manager->GetSensorsCount ());
      
    return (sensor_t)manager->CreateHandler (sensor_index);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::CreateSensor");
    throw;
  }
}

void TabletOsSensorManager::DestroySensor (sensor_t sensor)
{
  TabletOsSensorManagerImplSingleton::Instance ()->DestroyHandler ((TabletOsSensorHandler*)sensor);
}

/*
    Имя сенсора
*/

stl::string TabletOsSensorManager::GetSensorName (sensor_t sensor)
{
  return GetSensorType (sensor);
}

/*
    Производитель сенсора
*/

stl::string TabletOsSensorManager::GetSensorVendor (sensor_t)
{
  return "blackbarry";
}

/*
    Тип устройства
*/

stl::string TabletOsSensorManager::GetSensorType (sensor_t handle)
{
  try
  {
    if (!handle)   
      throw xtl::make_null_argument_exception ("", "handle");    
      
    int type = ((TabletOsSensorHandler*)handle)->GetType ();
    
    switch (type)
    {
      case SENSOR_TYPE_ACCELEROMETER:       return "Accelerometer";
      case SENSOR_TYPE_MAGNETOMETER:        return "MagneticField";
      case SENSOR_TYPE_GYROSCOPE:           return "Gyroscope";
      case SENSOR_TYPE_AZIMUTH_PITCH_ROLL:  return "Orientation";
      case SENSOR_TYPE_ALTIMETER:           return "Altimetr";
      case SENSOR_TYPE_TEMPERATURE:         return "Temperature"; 
      case SENSOR_TYPE_PROXIMITY:           return "Proximity";
      case SENSOR_TYPE_LIGHT:               return "Light";
      case SENSOR_TYPE_GRAVITY:             return "Gravity";                        
      case SENSOR_TYPE_LINEAR_ACCEL:        return "LinearAcceleration";
      case SENSOR_TYPE_ROTATION_VECTOR:     return "RotationVector";
      case SENSOR_TYPE_ROTATION_MATRIX:     return "RotationMatrix";
      default:                              return common::format ("tabletos_sensor_type%02d", type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::GetSensorType");
    throw;
  }
}

/*
    Максимальное значение
*/

float TabletOsSensorManager::GetSensorMaxRange (sensor_t sensor)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    return ((TabletOsSensorHandler*)sensor)->MaxRange ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::GetSensorType");
    throw;
  }
}

/*
    Частота обновления
*/

void TabletOsSensorManager::SetSensorUpdateRate (sensor_t sensor, float rate)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    ((TabletOsSensorHandler*)sensor)->SetRate (rate);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::GetSensorType");
    throw;
  }
}

float TabletOsSensorManager::GetSensorUpdateRate (sensor_t sensor)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    return ((TabletOsSensorHandler*)sensor)->GetRate ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::GetSensorType");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* TabletOsSensorManager::GetNativeSensorHandle (sensor_t)
{
  return 0;
}

/*
    Получение платформо-зависимых свойств
*/

void TabletOsSensorManager::GetSensorProperties (sensor_t sensor, common::PropertyMap& properties)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    ((TabletOsSensorHandler*)sensor)->GetProperties (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::GetSensorType");
    throw;
  }
}

/*
    Чтение событий сенсора
*/

void TabletOsSensorManager::StartSensorPolling (sensor_t, ISensorEventListener&)
{
}

void TabletOsSensorManager::StopSensorPolling (sensor_t)
{
}

void TabletOsSensorManager::PollSensorEvents (sensor_t)
{
}

}