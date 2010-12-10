#include "shared.h"

using namespace render::mid_level;

/*
    Описание реализации менеджера устройства
*/

struct DeviceManager::Impl
{
  LowLevelDevicePtr device;           //устройство визуализации
  stl::string       driver_name;      //имя драйвера
  stl::string       adapter_name;     //имя адаптера
  GeometryManager*  geometry_manager; //менеджер геометрии
  
  Impl (const LowLevelDevicePtr& in_device, const char* in_driver_name, const char* in_adapter_name)
    : device (in_device)
    , driver_name (in_driver_name)
    , adapter_name (in_adapter_name)
    , geometry_manager (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

DeviceManager::DeviceManager (const LowLevelDevicePtr& device, const char* driver_name, const char* adapter_name)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!driver_name)
      throw xtl::make_null_argument_exception ("", "driver_name");

    if (!adapter_name)
      throw xtl::make_null_argument_exception ("", "adapter_name");

    impl = new Impl (device, driver_name, adapter_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::DeviceManager::DeviceManager");
    throw;
  }
}

DeviceManager::~DeviceManager ()
{
}

/*
    Получение устройства
*/

render::low_level::IDevice& DeviceManager::Device ()
{
  return *impl->device;
}

/*
    Имя драйвера и адаптера
*/

const char* DeviceManager::DriverName ()
{
  return impl->driver_name.c_str ();
}

const char* DeviceManager::AdapterName ()
{
  return impl->adapter_name.c_str ();
}

/*
    Установка и чтение ссылки на менеджер геометрии
*/

render::mid_level::GeometryManager* DeviceManager::GeometryManager ()
{
  return impl->geometry_manager;
}

void DeviceManager::SetGeometryManager (GeometryManager* geometry_manager)
{
  impl->geometry_manager = geometry_manager;
}
