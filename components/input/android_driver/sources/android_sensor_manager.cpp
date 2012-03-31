#include "shared.h"

using namespace input::low_level::android_driver;

/*
    Вспомогательные структуры
*/

namespace
{

struct SensorDesc
{
  ASensorRef handle; //дескриптор сенсора
  int        type;   //тип сенсора
};

typedef stl::vector<SensorDesc> SensorList;

}

/*
    Описание реализации менеджера сенсоров    
*/

struct SensorManager::Impl
{
  SensorList sensors; //сенсоры

///Конструктор
  Impl ()
  {
    ASensorManager* manager = ASensorManager_getInstance ();
    
    if (!manager)
      throw xtl::format_operation_exception ("", "::ASensorManager_getInstance failed");
      
    ASensor const* sensor = ASensorManager_getDefaultSensor(manager, ASENSOR_TYPE_GYROSCOPE);  
    
    printf ("!!!!!!!!!!!!!!!!!!!!!sensor is %p name='%s' vendor='%s' resolution=%f\n", sensor, ASensor_getName (sensor), ASensor_getVendor (sensor), ASensor_getResolution (sensor));
/*      
    ASensorList sensors = 0;    

    int result = ASensorManager_getSensorList (manager, &sensors);
    
    if (result < 0)
      throw xtl::format_operation_exception ("", "ASensorManager_getSensorList failed");      
      
    printf ("RESULT IS %d\n", result); fflush (stdout);
      
    for (int i=0; i<result; i++)
      printf ("REF IS %p\n", sensors [i]), fflush (stdout);*/
  }
};

/*
    Конструктор / деструктор
*/

SensorManager::SensorManager ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::android::SensorManager::SensorManager");
    throw;
  }
}

SensorManager::~SensorManager ()
{
}

/*
    Количество сенсоров
*/

size_t SensorManager::SensorsCount ()
{
  return impl->sensors.size ();
}

/*
    Создание сенсора
*/

Sensor* SensorManager::CreateSensor (size_t index)
{
  try
  {
    if (index >= impl->sensors.size ())
      throw xtl::make_range_exception ("", "index", index, impl->sensors.size ());

    return new Sensor (impl->sensors [index].handle, SensorName (index)); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::android::SensorManager::CreateSensor");
    throw;
  }
}

/*
    Имя сенсора
*/

const char* SensorManager::SensorName (size_t index)
{
  try
  {
    if (index >= impl->sensors.size ())
      throw xtl::make_range_exception ("", "index", index, impl->sensors.size ());

    switch (impl->sensors [index].type)
    {
      case ASENSOR_TYPE_ACCELEROMETER:  return "accelerometer";
      case ASENSOR_TYPE_MAGNETIC_FIELD: return "magnetic_field";
      case ASENSOR_TYPE_GYROSCOPE:      return "gyroscope";
      case ASENSOR_TYPE_LIGHT:          return "light";
      case ASENSOR_TYPE_PROXIMITY:      return "proximity";
      default:                          return "";
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::android::SensorManager::SensorName");
    throw;
  }
}

const char* SensorManager::SensorFullName (size_t index)
{
  try
  {
    if (index >= impl->sensors.size ())
      throw xtl::make_range_exception ("", "index", index, impl->sensors.size ());

    return ASensor_getName (impl->sensors [index].handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::low_level::android::SensorManager::SensorFullName");
    throw;
  }
}
