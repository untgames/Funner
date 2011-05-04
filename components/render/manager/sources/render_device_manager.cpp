#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера устройства
*/

struct DeviceManager::Impl
{
  LowLevelDevicePtr                device;                     //устройство визуализации
  LowLevelDriverPtr                driver;                     //драйвер устройства визуализации
  render::InputLayoutManager       input_layout_manager;       //менеджер лэйаутов геометрии
  render::ProgramParametersManager program_parameters_manager; //менеджер параметров программ шэйдинга
  
  Impl (const LowLevelDevicePtr& in_device, const LowLevelDriverPtr& in_driver)
    : device (in_device)
    , driver (in_driver)
    , input_layout_manager (in_device)
    , program_parameters_manager (in_device)
  {
  }
};

/*
    Конструктор / деструктор
*/

DeviceManager::DeviceManager (const LowLevelDevicePtr& device, const LowLevelDriverPtr& driver)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!driver)
      throw xtl::make_null_argument_exception ("", "driver");

    impl = new Impl (device, driver);
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
    Получение устройства / драйвера
*/

render::low_level::IDevice& DeviceManager::Device ()
{
  return *impl->device;
}

render::low_level::IDriver& DeviceManager::Driver ()
{
  return *impl->driver;
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
