#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    Реализация менеджера конфигураций
*/

class ConfigurationManagerImpl
{
  public:
///Получение конфигурации
    Configuration::Pointer GetConfiguration (const char* file_name)
    {
      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");
          
        ConfigurationMap::iterator iter = configurations.find (file_name);
        
        if (iter != configurations.end ())
          return iter->second; //конфигурация найдена в кэше
          
          //загрузка новой конфигурации

        Configuration::Pointer configuration (new Configuration (file_name), false);

        configurations.insert_pair (file_name, configuration);

        return configuration;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("scene_graph::physics::box2d::ConfigurationManagerImpl::GetConfiguration");
        throw;
      }
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, Configuration::Pointer> ConfigurationMap;

  private:
    ConfigurationMap configurations; //карта загруженных конфигураций
};

typedef common::Singleton<ConfigurationManagerImpl> ConfigurationManagerSingleton;

}

/*
    Загрузка конфигурации
*/

Configuration::Pointer ConfigurationManager::GetConfiguration (const char* file_name)
{
  return ConfigurationManagerSingleton::Instance ()->GetConfiguration (file_name);
}
