#include "shared.h"

namespace syslib
{

/*
    Генерация исключения: работа с сенсорами невозможна
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No sensors support for this platform");
}

}

/*
    Количество сенсоров
*/

size_t DefaultSensorManager::GetSensorsCount ()
{
  return 0;
}

/*
    Создание / удаление сенсора
*/

sensor_t DefaultSensorManager::CreateSensor (size_t sensor_index)
{
  raise ("syslib::DefaultSensorManager::CreateSensor");
  return 0;
}

void DefaultSensorManager::DestroySensor (sensor_t)
{
  raise ("syslib::DefaultSensorManager::DestroySensor");
}

/*
    Имя сенсора
*/

stl::string DefaultSensorManager::GetSensorName (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorName");
  return "";
}

/*
    Производитель сенсора
*/

stl::string DefaultSensorManager::GetSensorVendor (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorVendor");
  return "";
}

/*
    Тип устройства
*/

stl::string DefaultSensorManager::GetSensorType (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorType");
  return "";
}

/*
    Максимальное значение
*/

float DefaultSensorManager::GetSensorMaxRange (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorMaxRange");
  return 0;
}

/*
    Частота обновления
*/

void DefaultSensorManager::SetSensorUpdateRate (sensor_t, float rate)
{
  raise ("syslib::DefaultSensorManager::SetSensorUpdateRate");
}

float DefaultSensorManager::GetSensorUpdateRate (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetSensorUpdateRate");
  return 0;
}

/*
    Получение платформо-зависимого дескриптора экрана
*/

const void* DefaultSensorManager::GetNativeSensorHandle (sensor_t)
{
  raise ("syslib::DefaultSensorManager::GetNativeSensorHandle");
  return 0;
}

/*
    Получение платформо-зависимых свойств экрана
*/

void DefaultSensorManager::GetSensorProperties (sensor_t, common::PropertyMap& properties)
{
  raise ("syslib::DefaultSensorManager::GetSensorProperties");
}

/*
    Чтение событий сенсора
*/

void DefaultSensorManager::StartSensorPolling (sensor_t, ISensorEventListener&)
{
  raise ("syslib::DefaultSensorManager::StartSensorPolling");
}

void DefaultSensorManager::StopSensorPolling (sensor_t)
{
  raise ("syslib::DefaultSensorManager::StopSensorPolling");
}

void DefaultSensorManager::PollSensorEvents (sensor_t)
{
}

}
