#include "shared.h"

using namespace render;
using namespace render::render2d;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME   = "render.scene_render.Render2d"; //имя компонента
const char* RENDER_PATH_NAME = "Render2d";                     //имя пути рендеринга

/*
    Компонент рендера двумерной сцены
*/

class SceneRender2dComponent
{
  public:
    SceneRender2dComponent ()
    {
      SceneRenderManager::RegisterRender (RENDER_PATH_NAME, &CreateRender);
    }

  private:
    static ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer, const char*)
    {
      return new render2d::Render (renderer);
    }
};

}

extern "C" 
{

common::ComponentRegistrator<SceneRender2dComponent> SceneRender2d (COMPONENT_NAME);

}
