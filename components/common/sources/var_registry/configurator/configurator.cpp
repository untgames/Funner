#include <xtl/exception.h>

#include <common/component.h>
#include <common/log.h>
#include <common/var_registry.h>

namespace
{

const char* DEFAULT_CONFIGURATION_FILE_NAME = "config.xml";
const char* LOG_NAME = "common.Configurator";

/*
   Компонент загрузки конфигураций
*/

class ConfiguratorComponent
{
  public:
    //загрузка компонента
    ConfiguratorComponent () 
    {
      try
      {
        common::ConfigurationRegistry::LoadConfiguration (DEFAULT_CONFIGURATION_FILE_NAME);
      }
      catch (xtl::exception& e)
      {
        common::LogSystem::Print (LOG_NAME, e.what ());
      }
    }
};

extern "C"
{

common::ComponentRegistrator<ConfiguratorComponent> Configurator ("common.var_registries.configuration_registry");

}

}
