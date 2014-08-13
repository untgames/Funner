#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Конструктор / деструктор
*/

BasicTechnique::BasicTechnique (RenderManager& in_manager, const common::ParseNode& node, bool read_effect_name)
  : manager (in_manager)
{
    //чтение имени эффекта

  if (read_effect_name)
    effect_name = common::get<const char*> (node, "effect");
}    

BasicTechnique::~BasicTechnique ()
{
}

/*
    Создание кадра
*/

manager::Frame BasicTechnique::CreateFrame (PrivateData& private_data, const char* overrided_effect_name)
{
  try
  {
    manager::Frame frame = manager.Manager ().CreateFrame ();

      //задание эффекта

    frame.SetEffect (overrided_effect_name ? overrided_effect_name : effect_name.c_str ());

      //задание начальных свойств кадра

    private_data.frame_properties.SetProperty ("ViewMatrix",       math::mat4f (1.0f));
    private_data.frame_properties.SetProperty ("ProjectionMatrix", math::mat4f (1.0f));

    private_data.view_matrix_property_index = private_data.frame_properties.IndexOf ("ViewMatrix");
    private_data.proj_matrix_property_index = private_data.frame_properties.IndexOf ("ProjectionMatrix");

      //задание начальных свойств для пар frame-entity

    private_data.frame_entity_properties.SetProperty ("ModelViewMatrix",           math::mat4f (1.0f));
    private_data.frame_entity_properties.SetProperty ("ModelViewProjectionMatrix", math::mat4f (1.0f));
    private_data.frame_entity_properties.SetProperty ("ObjectMatrix",              math::mat4f (1.0f));

    private_data.mv_matrix_property_index     = private_data.frame_entity_properties.IndexOf ("ModelViewMatrix");
    private_data.mvp_matrix_property_index    = private_data.frame_entity_properties.IndexOf ("ModelViewProjectionMatrix");
    private_data.object_matrix_property_index = private_data.frame_entity_properties.IndexOf ("ObjectMatrix");

    frame.SetEntityDependentProperties (private_data.frame_entity_properties);
    frame.SetEntityDrawHandler (xtl::bind (&BasicTechnique::EntityDrawHandler, this, _1, _2, _3, _4));

      //задание свойств фрейма

    frame.SetProperties (private_data.frame_properties);

    return frame;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicTechnique::CreateFrame");
    throw;
  }
}

/*
    Обновление кадра
*/

void BasicTechnique::ConfigureFrame (RenderingContext& parent_context, PrivateData& private_data)
{
  try
  {
      //обновление параметров камеры

    if (private_data.view_proj_transaction_id != parent_context.Camera ().ViewProjectionMatrixTransactionId ())
    {
      private_data.view_proj_tm             = parent_context.Camera ().ViewProjectionMatrix ();
      private_data.view_proj_transaction_id = parent_context.Camera ().ViewProjectionMatrixTransactionId ();

      private_data.frame.SetViewProjectionMatrix (private_data.view_proj_tm);
    }

    if (private_data.view_transaction_id != parent_context.Camera ().ViewMatrixTransactionId ())
    {
      private_data.view_tm             = parent_context.Camera ().ViewMatrix ();
      private_data.view_transaction_id = parent_context.Camera ().ViewMatrixTransactionId ();

      private_data.frame_properties.SetProperty (private_data.view_matrix_property_index, parent_context.Camera ().ViewMatrix ());
    }

    if (private_data.proj_transaction_id != parent_context.Camera ().ProjectionMatrixTransactionId ())
    {
      private_data.proj_transaction_id = parent_context.Camera ().ProjectionMatrixTransactionId ();

      private_data.frame_properties.SetProperty (private_data.proj_matrix_property_index, parent_context.Camera ().ProjectionMatrix ());
    }
      
      //обновление свойств объект-кадр (для рендеринга спрайтов)

    private_data.frame_entity_properties.SetProperty (private_data.mv_matrix_property_index,  private_data.view_proj_tm);
    private_data.frame_entity_properties.SetProperty (private_data.mvp_matrix_property_index, private_data.view_proj_tm);
    private_data.frame_entity_properties.SetProperty (private_data.object_matrix_property_index, math::mat4f (1.0f));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicTechnique::ConfigureFrame");
    throw;
  }
}

void BasicTechnique::DrawVisualModels (RenderingContext& parent_context, PrivateData& private_data)
{
  try
  {
      //построение списка моделей на отрисовку

    TraverseResult& result = parent_context.TraverseResult ();
    
      //обновление визуализируемых объектов

    RenderingContext context (parent_context, private_data.frame);

    Technique::Draw (context, result.visual_models, &private_data);

      //добавление дочернего кадра

    parent_context.Frame ().AddFrame (private_data.frame);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicTechnique::DrawVisualModels");
    throw;
  }
}

void BasicTechnique::UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
{
  try
  {
      //получение приватных данных техники

    PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

      //настройка кадра

    ConfigureFrame (parent_context, private_data);

      //отрисовка моделей

    DrawVisualModels (parent_context, private_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicTechnique::UpdateFrameCore");
    throw;
  }
}

/*
    Обработчик отрисовки объектов
*/

void BasicTechnique::EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params)
{
  PrivateData& private_data = *reinterpret_cast<PrivateData*> (user_data);
  VisualModel& model        = *reinterpret_cast<VisualModel*> (entity.UserData ());

  if (!&private_data || !&model)
    return;

  ConfigureEntity (frame, entity, model, private_data, out_params);
}

void BasicTechnique::ConfigureEntity (manager::Frame& frame, manager::Entity& entity, VisualModel& model, PrivateData& private_data, manager::EntityDrawParams& out_params)
{
  common::PropertyMap& properties = out_params.properties;
  const math::mat4f&   object_tm  = model.WorldMatrix ();

  out_params.mvp_matrix = private_data.view_proj_tm * object_tm;

  properties.SetProperty (private_data.mv_matrix_property_index,  private_data.view_tm * object_tm);
  properties.SetProperty (private_data.mvp_matrix_property_index, out_params.mvp_matrix);
  properties.SetProperty (private_data.object_matrix_property_index, object_tm);
}

/*
    Обновление свойств / связывание свойств техники с методами техники
*/

void BasicTechnique::UpdatePropertiesCore ()
{
}

void BasicTechnique::BindProperties (common::PropertyBindingMap&)
{
}
