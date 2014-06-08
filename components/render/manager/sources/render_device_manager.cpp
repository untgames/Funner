#include "shared.h"

using namespace render;
using namespace render::manager;

/*
    Описание реализации менеджера устройства
*/

struct DeviceManager::Impl
{
  LowLevelDeviceContextPtr                  context;                    //непосредственный контекст отрисовки
  LowLevelDevicePtr                         device;                     //устройство визуализации
  LowLevelDriverPtr                         driver;                     //драйвер устройства визуализации
  CacheManagerPtr                           cache_manager;              //менеджер кэширования
  render::manager::InputLayoutManager       input_layout_manager;       //менеджер лэйаутов геометрии
  render::manager::ProgramParametersManager program_parameters_manager; //менеджер параметров программ шэйдинга  
  SettingsPtr                               settings;                   //настройки менеджера рендеринга
  low_level::DeviceCaps                     device_caps;                //возможности устройства отрисовки
  
  Impl (const LowLevelDevicePtr& in_device, const LowLevelDriverPtr& in_driver, const SettingsPtr& in_settings, const CacheManagerPtr& in_cache_manager)
    : context (in_device->GetImmediateContext ())
    , device (in_device)
    , driver (in_driver)
    , cache_manager (in_cache_manager)
    , input_layout_manager (in_device, in_settings)
    , program_parameters_manager (in_device, in_settings, in_cache_manager)
    , settings (in_settings)
  {
    device->GetCaps (device_caps);    
  }
};

/*
    Конструктор / деструктор
*/

DeviceManager::DeviceManager (const LowLevelDevicePtr& device, const LowLevelDriverPtr& driver, const SettingsPtr& settings, const CacheManagerPtr& cache_manager)
{
  try
  {
    if (!device)
      throw xtl::make_null_argument_exception ("", "device");

    if (!driver)
      throw xtl::make_null_argument_exception ("", "driver");
      
    if (!settings)
      throw xtl::make_null_argument_exception ("", "settings");
      
    if (!cache_manager)
      throw xtl::make_null_argument_exception ("", "cache_manager");

    impl = new Impl (device, driver, settings, cache_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DeviceManager::DeviceManager");
    throw;
  }
}

DeviceManager::~DeviceManager ()
{
}

/*
    Получение устройства / непосредственного контекста / драйвера
*/

render::low_level::IDevice& DeviceManager::Device ()
{
  return *impl->device;
}

render::low_level::IDeviceContext& DeviceManager::ImmediateContext ()
{
  return *impl->context;
}

render::low_level::IDriver& DeviceManager::Driver ()
{
  return *impl->driver;
}

/*
    Возможности устройства отрисовки
*/

const low_level::DeviceCaps& DeviceManager::DeviceCaps ()
{
  return impl->device_caps;
}

/*
    Менеджер кэширования
*/

render::manager::CacheManager& DeviceManager::CacheManager ()
{
  return *impl->cache_manager;
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

/*
    Настройки менеджера рендеринга
*/

Settings& DeviceManager::Settings ()
{
  return *impl->settings;
}
