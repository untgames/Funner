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
class DrawWithoutLights: public Technique
{
  public:
///Конструктор / деструктор
    DrawWithoutLights (RenderManager& in_manager, const common::ParseNode& node)
      : manager (in_manager)
    {
        //чтение имени эффекта

      effect_name = common::get<const char*> (node, "effect");
    }

/// Имена для регистрации
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  protected:
///Данные для рендеринга техники в каждом кадре
    struct PrivateData
    {
      BasicRendererPtr renderer;

      PrivateData (DrawWithoutLights& technique) : renderer (new BasicRenderer (technique.manager, technique.effect_name.c_str ()), false) {}
    };

///Обновление кадра
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //получение приватных данных техники

        PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

          //обновление кадра

        private_data.renderer->Draw (parent_context);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///Обновление свойств
    void UpdatePropertiesCore () {}

///Связывание свойств техники с методами техники
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    RenderManager manager;     //менеджер рендеринга
    stl::string   effect_name; //имя эффекта
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<render::scene::server::DrawWithoutLights> DrawWithoutLights;

}
