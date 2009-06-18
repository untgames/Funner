#include "shared.h"

using namespace common;

namespace
{

const char* BRANCH_NAME = "Configuration";

class ConfigurationRegistryImpl
{
  public:
    ConfigurationRegistryImpl ()
      : var_registry (BRANCH_NAME)
    {
      string_registry_container.Mount (BRANCH_NAME);
    }

    void LoadConfiguration (const char* file_name_mask)
    {
      try
      {
        load_xml_configuration (var_registry, file_name_mask, BRANCH_NAME);
      }
      catch (xtl::exception& e)
      {
        e.touch ("common::ConfigurationRegistry::LoadConfiguration");
        throw;
      }
    }

    void UnloadAll ()
    {
      string_registry_container.RemoveAllVariables ();
    }

  private:
    VarRegistry                       var_registry;
    VarRegistryContainer<stl::string> string_registry_container;
};

typedef Singleton<ConfigurationRegistryImpl> ConfigurationRegistrySingleton;

}

/*
   Конфигурационный реестр
*/

/*
   Имя ветки реестра с конфигурациями
*/

const char* ConfigurationRegistry::BranchName ()
{
  return BRANCH_NAME;
}

/*
   Загрузка конфигурации
*/

void ConfigurationRegistry::LoadConfiguration (const char* file_name_mask)
{
  ConfigurationRegistrySingleton::Instance ()->LoadConfiguration (file_name_mask);
}

/*
   Сброс конфигураций
*/

void ConfigurationRegistry::UnloadAll ()
{
  ConfigurationRegistrySingleton::Instance ()->UnloadAll ();
}
