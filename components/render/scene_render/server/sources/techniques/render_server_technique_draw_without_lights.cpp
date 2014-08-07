#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

namespace render
{

namespace scene
{

namespace server
{

///Техника рендеринга: отрисовка изображения без освещения
class DrawWithoutLights: public BasicTechnique
{
  public:
///Конструктор / деструктор
    DrawWithoutLights (RenderManager& manager, const common::ParseNode& node)
      : BasicTechnique (manager, node)
    {
    }    

/// Имена для регистрации
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::DrawWithoutLights> DrawWithoutLights;

}
