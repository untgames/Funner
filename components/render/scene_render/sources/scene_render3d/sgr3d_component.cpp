#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace components
{

namespace scene_render3d
{

/*
    Константы
*/

const char* COMPONENT_NAME = "render.scene.render3d"; //имя компонента
const char* RENDER_NAME    = "render3d";              //имя рендера

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      SceneRenderManager::RegisterRender (RENDER_NAME, &Component::CreateRender);
    }
    
  private:
    static ISceneRender* CreateRender (RenderManager& manager, const char*)
    {
      return new Render (manager);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SceneRender3D (COMPONENT_NAME);

}

}

}
