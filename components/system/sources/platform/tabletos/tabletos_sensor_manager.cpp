#include "shared.h"

using namespace syslib;
using namespace syslib::tabletos;

namespace
{

class TabletOsSensor : public ISensorImpl
{
  friend class TabletOsSensorManagerImpl;
  friend class TabletOsSensorManager;

  public:
    TabletOsSensor (sensor_type_t t):
      type (t),
      rate (25000),
      listener ()
    {
      try
      {
        if (sensor_info (type, &info) == BPS_FAILURE)
          tabletos::raise_error ("::sensor_info");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensor::TabletOsSensor");
        throw;
      }
    }
 
    ~TabletOsSensor ()
    {
      try
      {
        if (sensor_info_destroy (info) == BPS_FAILURE);
          tabletos::raise_error ("::sensor_info_destroy");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensor::~TabletOsSensor");
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
        rate = r;
        if (sensor_set_rate (type, r) == BPS_FAILURE);
          tabletos::raise_error ("::sensor_set_rate");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensor::SetRate");
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

    void StartSensorPolling (ISensorEventListener &l)
    {
      try
      {
        if (sensor_set_rate (type, rate) == BPS_FAILURE)
          tabletos::raise_error ("::sensor_set_rate");

        if (sensor_set_skip_duplicates (type, true) == BPS_FAILURE)
          tabletos::raise_error ("::sensor_set_skip_duplicates");

        if (sensor_request_events (type) == BPS_FAILURE)
         tabletos::raise_error ("::sensor_request_events");
     
        listener = &l;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensor::StartSensorPolling");
        throw;
      }
    }

    void StopSensorPolling ()
    {
      try
      {
        listener = 0;
        if ( sensor_stop_events (type) == BPS_FAILURE);
          raise_error ("::sensor_stop_events");
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensor::StopSensorPolling");
        throw;
      }
    }
         
    void OnSensorEvent (int event_type, bps_event_t *event)
    {
      try
      {
        if (!listener)
          return;

        SensorEvent sensor_event;
        sensor_event.timestamp = common::milliseconds ();

        switch (type)
        {
          case SENSOR_TYPE_ACCELEROMETER:
          case SENSOR_TYPE_MAGNETOMETER:       
          case SENSOR_TYPE_GYROSCOPE:          
          case SENSOR_TYPE_LINEAR_ACCEL:
          case SENSOR_TYPE_GRAVITY:            
            sensor_event.values_count = 3;
            if (sensor_event_get_xyz (event, &sensor_event.data[0], &sensor_event.data[1], &sensor_event.data[2]) == BPS_FAILURE)
              raise_error ("::sensor_event_get_xyz");
            break;      

          case SENSOR_TYPE_AZIMUTH_PITCH_ROLL: 
            sensor_event.values_count = 3;
            if (sensor_event_get_apr (event, &sensor_event.data[0], &sensor_event.data[1], &sensor_event.data[2]))       
              raise_error ("::sensor_event_get_apr");
            break;      
          case SENSOR_TYPE_ALTIMETER:          
            sensor_event.values_count = 1;
            sensor_event.data[0] = sensor_event_get_altitude (event);
            break;      
          case SENSOR_TYPE_TEMPERATURE:        
            sensor_event.values_count = 1;
            sensor_event.data[0] = sensor_event_get_temperature (event);
            break;      
          case SENSOR_TYPE_PROXIMITY:          
            sensor_event.values_count = 1;
            sensor_event.data[0] = sensor_event_get_proximity (event);
            break;      
          case SENSOR_TYPE_LIGHT:              
            sensor_event.values_count = 1;
            sensor_event.data[0] = sensor_event_get_illuminance (event);
            break;      
          case SENSOR_TYPE_ROTATION_VECTOR:    
            sensor_event.values_count = 4;
            if (sensor_event_get_rotation_vector (event, (sensor_rotation_vector_t*)sensor_event.data))       
              raise_error ("::sensor_event_get_rotation_vector");
            break;      
          case SENSOR_TYPE_ROTATION_MATRIX:    
            sensor_event.values_count = 9;
            if (sensor_event_get_rotation_matrix (event, (sensor_rotation_matrix_t*)sensor_event.data))       
              raise_error ("::sensor_event_get_rotation_matrix");
            break;      
          default:                             
            break;      
        }

        listener->OnSensorChanged (sensor_event);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensor::OnSensorEvent");
        throw;
      }    
    }
           
  private:
    sensor_type_t type;
    sensor_info_t *info;
    unsigned int rate;
    ISensorEventListener *listener;
};

class TabletOsSensorManagerImpl
{
  public:
    TabletOsSensorManagerImpl ()
    {
      static sensor_type_t sens[] = {SENSOR_TYPE_ACCELEROMETER,SENSOR_TYPE_MAGNETOMETER,SENSOR_TYPE_GYROSCOPE,
        SENSOR_TYPE_AZIMUTH_PITCH_ROLL,SENSOR_TYPE_ALTIMETER,SENSOR_TYPE_TEMPERATURE,SENSOR_TYPE_PROXIMITY,
        SENSOR_TYPE_LIGHT,SENSOR_TYPE_GRAVITY,SENSOR_TYPE_LINEAR_ACCEL,SENSOR_TYPE_ROTATION_VECTOR,
        SENSOR_TYPE_ROTATION_MATRIX};

      platform_initialize ();

      for (unsigned int i=0; i<sizeof (sens) / sizeof (*sens); i++)
        if (sensor_is_supported (sens [i]))
          avalible_sensors.push_back (sens [i]);
    }

    size_t GetSensorsCount ()
    {
      return avalible_sensors.size ();
    }

    TabletOsSensor *CreateHandler (size_t index)
    {
      TabletOsSensor *handler = new TabletOsSensor (avalible_sensors [index]);
      return handler;
    } 

    void DestroyHandler (TabletOsSensor *handler)
    {
      delete handler;
    }

    void RegisterSensor (sensor_type_t type, ISensorImpl *impl)
    {
      try
      {
        if (!impl)
          throw xtl::make_null_argument_exception ("", "impl");

        if (FindSensor (type) !=0)
          raise_error ("Register more than one sensor same type unsupported");

        sensors.insert_pair (type, impl);
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensorManagerImpl::RegisterSensor");
        throw;
      }    
    }

    void UnregisterSensor (sensor_type_t type)
    {
      sensors.erase (type);
    }

    ISensorImpl* FindSensor (sensor_type_t sensor)
    {
      try
      {
        SensorMap::iterator iter = sensors.find (sensor);

        if (iter == sensors.end ())
          return 0;

        return iter->second;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::TabletOsSensorManagerImpl::FindSensor (sensor)");
        throw;
      }
    }

  private:
    typedef stl::vector<sensor_type_t> SensorVector;
    typedef stl::hash_map<sensor_type_t, ISensorImpl*> SensorMap;

  private:
    SensorVector avalible_sensors;
    SensorMap sensors;
};

typedef common::Singleton<TabletOsSensorManagerImpl> TabletOsSensorManagerImplSingleton;
}

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
  TabletOsSensorManagerImplSingleton::Instance ()->DestroyHandler ((TabletOsSensor*)sensor);
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
      
    int type = ((TabletOsSensor*)handle)->GetType ();
    
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

    return ((TabletOsSensor*)sensor)->MaxRange ();
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

    ((TabletOsSensor*)sensor)->SetRate (rate);
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

    return ((TabletOsSensor*)sensor)->GetRate ();
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

const void* TabletOsSensorManager::GetNativeSensorHandle (sensor_t sensor)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    return (void*)((TabletOsSensor*)sensor)->GetType ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::GetNativeSensorHandle");
    throw;
  }
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

    ((TabletOsSensor*)sensor)->GetProperties (properties);
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

void TabletOsSensorManager::StartSensorPolling (sensor_t sensor, ISensorEventListener &listener)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    TabletOsSensor *s = (TabletOsSensor*)sensor;
    s->StartSensorPolling (listener);

    TabletOsSensorManagerImplSingleton::Instance manager;
    manager->RegisterSensor (s->GetType (), s);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::StopSensorPolling");
    throw;
  }
}

void TabletOsSensorManager::StopSensorPolling (sensor_t sensor)
{
  try
  {
    if (!sensor)   
      throw xtl::make_null_argument_exception ("", "sensor");

    TabletOsSensor *s = (TabletOsSensor*)sensor;
    s->StopSensorPolling ();

    TabletOsSensorManagerImplSingleton::Instance manager;
    manager->UnregisterSensor (s->GetType ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsSensorManager::StopSensorPolling");
    throw;
  }
}

void TabletOsSensorManager::PollSensorEvents (sensor_t)
{
}



/*
    Sensor registry (used for )
*/

void SensorRegistry::RegisterSensor (sensor_type_t sensor, ISensorImpl* impl)
{
  TabletOsSensorManagerImplSingleton::Instance manager;
  manager->RegisterSensor (sensor,impl);
}

void SensorRegistry::UnregisterSensor (sensor_type_t sensor)
{
  TabletOsSensorManagerImplSingleton::Instance manager;
  manager->UnregisterSensor (sensor);
}    

ISensorImpl* SensorRegistry::FindSensor (sensor_type_t sensor)
{
  TabletOsSensorManagerImplSingleton::Instance manager;
  return manager->FindSensor (sensor);
}

