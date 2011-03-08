#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера устройства
*/

struct DeviceManager::Impl
{
  LowLevelDevicePtr                device;                     //устройство визуализации
  stl::string                      driver_name;                //имя драйвера
  stl::string                      adapter_name;               //имя адаптера
  render::InputLayoutManager       input_layout_manager;       //менеджер лэйаутов геометрии
  render::ProgramParametersManager program_parameters_manager; //менеджер параметров программ шэйдинга
  
  Impl (const LowLevelDevicePtr& in_device, const char* in_driver_name, const char* in_adapter_name)
    : device (in_device)
    , driver_name (in_driver_name)
    , adapter_name (in_adapter_name)
    , input_layout_manager (in_device)
    , program_parameters_manager (in_device)
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
    e.touch ("render::DeviceManager::DeviceManager");
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
    Менеджер лэйаутов геометрии
*/

InputLayoutManager& DeviceManager::InputLayoutManager ()
{
  return impl->input_layout_manager;
}

/*
    Менеджер параметров программ шэйдинга
*/

ProgramParametersManager& DeviceManager::ProgramParametersManager ()
{
  return impl->program_parameters_manager;
}
