#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME  = "scene_graph.controllers.box2d_physics"; //имя компонента
const char* CONTROLLER_NAME = "Box2D.World";                           //имя контроллера физического мира

/*
    Регистрация компонента
*/

class Component
{
  public:
    Component ()
    {
      ControllerManager::Register (CONTROLLER_NAME, &CreateWorldController);
    }
    
  private:
    static IController* CreateWorldController (Node& node, const xtl::any_reference& param)
    {
      try
      {
        return new WorldController (node);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("scene_graph::box2d::Component::CreateWorldController");
        throw;
      }
    }
};

extern "C"
{

common::ComponentRegistrator<Component> Box2DController (COMPONENT_NAME);

}

}
