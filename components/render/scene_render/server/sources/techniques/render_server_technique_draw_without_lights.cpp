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
      , entity_draw_handler (xtl::bind (&DrawWithoutLights::EntityDrawHandler, this, _1, _2, _3))
      , properties_update_counter (1)
    {
      SetDefaultProperties (node);
    }    

/// Имена для регистрации
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  private:
    struct PrivateData
    {
      size_t properties_update_counter;

      PrivateData ()
        : properties_update_counter ()
      {
      }
    };

  
  private:
///Обновление кадра
    void UpdateFrameCore (RenderingContext& context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //получение приватных данных техники

        PrivateData& private_data = private_data_holder.Get<PrivateData> ();

        if (private_data.properties_update_counter != properties_update_counter)
        {
            //TODO: update properties

          private_data.properties_update_counter = properties_update_counter;
        }

          //построение списка моделей на отрисовку

        TraverseResult& result = context.TraverseResult ();

          //установка параметров фрейма

        //TODO: configure frame
        
          //обновление визуализируемых объектов

        Technique::Draw (context, result.visual_models);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///Обработчик отрисовки объектов
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, manager::EntityDrawParams& out_params)
    {
//    common::PropertyMap& properties = out_params.properties;
      
//    math::mat4f model_view_tm = frame.Properties ().GetMatrix ("ViewMatrix") * entity.Properties ().GetMatrix ("ObjectMatrix");  

//    out_params.mvp_matrix = frame.Properties ().GetMatrix ("ProjectionMatrix") * model_view_tm;

//    properties.SetProperty ("ModelViewMatrix", model_view_tm);
//    properties.SetProperty ("ModelViewProjectionMatrix", out_params.mvp_matrix);    
    }

///Обновление свойств
    void UpdatePropertiesCore ()
    {
      properties_update_counter++;
    }

///Связывание свойств техники с методами техники
    void BindProperties (common::PropertyBindingMap& map)
    {
      //TODO: bind properties
    }

  private:
    RenderManager                      manager;                   //менеджер рендеринга
    manager::Frame::EntityDrawFunction entity_draw_handler;       //обработчик отрисовки объектов
    size_t                             properties_update_counter; //счётчик обновлений свойств
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<DrawWithoutLights> DrawWithoutLights;

}
