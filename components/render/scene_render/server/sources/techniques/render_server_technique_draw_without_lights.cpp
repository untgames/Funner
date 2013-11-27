#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

//TODO: recursive rendering (move state to the technique)

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
      , frame (manager.Manager ().CreateFrame ())
      , entity_draw_handler (xtl::bind (&DrawWithoutLights::EntityDrawHandler, this, _1, _2, _3, _4))
    {
        //задание начальных свойств для пар frame-entity

      common::PropertyMap properties;

      properties.SetProperty ("ModelViewMatrix",           math::mat4f (1.0f));
      properties.SetProperty ("ModelViewProjectionMatrix", math::mat4f (1.0f));

      mv_matrix_property_index   = properties.IndexOf ("ModelViewMatrix");
      mvp_matrix_property_index  = properties.IndexOf ("ModelViewProjectionMatrix");      

      frame.SetInitialEntityDrawProperties (properties);

        //задание начальных свойств кадра

      frame_properties.SetProperty ("ViewMatrix", math::mat4f (1.0f));

      frame.SetProperties (frame_properties);

      view_matrix_property_index = frame_properties.IndexOf ("ViewMatrix");
    }    

/// Имена для регистрации
    static const char* ClassName     () { return "draw_without_lights"; }
    static const char* ComponentName () { return "render.scene.server.techniques.draw_without_lights"; }

  private:
    struct PrivateData
    {
      size_t      view_transaction_id;
      size_t      view_proj_transaction_id;
      math::mat4f view_tm;
      math::mat4f view_proj_tm;

      PrivateData ()
        : view_transaction_id ()
        , view_proj_transaction_id ()
        , view_tm (1.0f)
        , view_proj_tm (1.0f)
      {
      }
    };
  
  private:
///Обновление кадра
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
    {
      try
      {
          //получение приватных данных техники

        PrivateData& private_data = private_data_holder.Get<PrivateData> ();

        if (private_data.view_proj_transaction_id != parent_context.Camera ().ViewProjectionMatrixTransactionId ())
        {
          private_data.view_proj_tm             = parent_context.Camera ().ViewProjectionMatrix ();
          private_data.view_proj_transaction_id = parent_context.Camera ().ViewProjectionMatrixTransactionId ();
        }

        if (private_data.view_transaction_id != parent_context.Camera ().ViewMatrixTransactionId ())
        {
          private_data.view_tm             = parent_context.Camera ().ViewMatrix ();
          private_data.view_transaction_id = parent_context.Camera ().ViewMatrixTransactionId ();

          frame_properties.SetProperty (view_matrix_property_index, parent_context.Camera ().ViewMatrix ());
        }

          //построение списка моделей на отрисовку

        TraverseResult& result = parent_context.TraverseResult ();
        
          //обновление визуализируемых объектов

        RenderingContext context (parent_context, frame);

        Technique::Draw (context, result.visual_models);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene_render3d::DrawWithoutLights::UpdateFrameCore");
        throw;
      }
    }

///Обработчик отрисовки объектов
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params)
    {
      common::PropertyMap& properties    = out_params.properties;
      PrivateData&         private_data  = *reinterpret_cast<PrivateData*> (user_data);
      VisualModel&         model         = *reinterpret_cast<VisualModel*> (entity.UserData ());

      if (!&private_data || !&model)
        return;

      const math::mat4f& object_tm = model.WorldMatrix ();
      
      out_params.mvp_matrix = private_data.view_proj_tm * object_tm;

      properties.SetProperty (mv_matrix_property_index,  private_data.view_tm * object_tm);
      properties.SetProperty (mvp_matrix_property_index, out_params.mvp_matrix);
    }

///Обновление свойств
    void UpdatePropertiesCore () {}

///Связывание свойств техники с методами техники
    void BindProperties (common::PropertyBindingMap&) {}

  private:
    RenderManager                      manager;                    //менеджер рендеринга
    manager::Frame                     frame;                      //фрейм техники
    manager::Frame::EntityDrawFunction entity_draw_handler;        //обработчик отрисовки объектов
    common::PropertyMap                frame_properties;           //свойства кадра
    size_t                             mv_matrix_property_index;   //индекс свойства матрицы ModelView
    size_t                             mvp_matrix_property_index;  //индекс свойства матрицы ModelViewProjection
    size_t                             view_matrix_property_index; //индекс свойства матрицы View (в свойствах кадра)
};

}

}

}

extern "C"
{

TechniqueComponentRegistrator<DrawWithoutLights> DrawWithoutLights;

}
