#include "shared.h"

using namespace scene_graph;

namespace components
{

namespace xscene_loader
{

class Component
{
  public:
    Component ()
    {
      SceneSerializationManager::RegisterLoader ("xscene", &Component::CreateFactory);
      XmlSceneParserManager::RegisterParser ("1.0", XmlSceneParser::Create);
    }
    
  private:
    static ISceneFactory* CreateFactory (const char* file_name, const SceneManager::LogHandler& log_handler)
    {
      return new XmlSceneFactory (file_name, log_handler);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> XSceneLoader ("scene.loaders.XScene");

}

}

}
