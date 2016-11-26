#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Конструктор / деструктор
*/

BasicRenderer::BasicRenderer (server::RenderManager& in_manager, const char* effect_name)
  : manager (in_manager)
  , frame (manager.Manager ().CreateFrame ())
  , view_transaction_id ()
  , proj_transaction_id ()
  , view_proj_transaction_id ()
  , view_tm (1.0f)
  , view_proj_tm (1.0f)
  , mv_matrix_property_index (~0u)
  , mvp_matrix_property_index (~0u)
  , object_matrix_property_index (~0u)
  , view_matrix_property_index (~0u)
  , proj_matrix_property_index (~0u)
{
  try
  {
      //задание эффекта

    frame.SetEffect (effect_name);

      //задание начальных свойств кадра

    frame_properties.SetProperty ("ViewMatrix",       math::mat4f (1.0f));
    frame_properties.SetProperty ("ProjectionMatrix", math::mat4f (1.0f));

    view_matrix_property_index = frame_properties.IndexOf ("ViewMatrix");
    proj_matrix_property_index = frame_properties.IndexOf ("ProjectionMatrix");

      //задание начальных свойств для пар frame-entity

    frame_entity_properties.SetProperty ("ModelViewMatrix",           math::mat4f (1.0f));
    frame_entity_properties.SetProperty ("ModelViewProjectionMatrix", math::mat4f (1.0f));
    frame_entity_properties.SetProperty ("ObjectMatrix",              math::mat4f (1.0f));

    mv_matrix_property_index     = frame_entity_properties.IndexOf ("ModelViewMatrix");
    mvp_matrix_property_index    = frame_entity_properties.IndexOf ("ModelViewProjectionMatrix");
    object_matrix_property_index = frame_entity_properties.IndexOf ("ObjectMatrix");

    frame.SetEntityDependentProperties (frame_entity_properties);
    frame.SetEntityDrawHandler         (xtl::bind (&BasicRenderer::EntityDrawHandler, this, _1, _2, _3, _4));

      //задание свойств фрейма

    frame.SetProperties (frame_properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicRenderer::BasicRenderer");
    throw;
  }
}    

BasicRenderer::~BasicRenderer ()
{
}

/*
    Обновление кадра
*/

void BasicRenderer::ConfigureFrame (RenderingContext& parent_context)
{
  try
  {
      //обновление параметров камеры

    if (view_proj_transaction_id != parent_context.Camera ().ViewProjectionMatrixTransactionId ())
    {
      view_proj_tm             = parent_context.Camera ().ViewProjectionMatrix ();
      view_proj_transaction_id = parent_context.Camera ().ViewProjectionMatrixTransactionId ();

      frame.SetViewProjectionMatrix (view_proj_tm);
    }

    if (view_transaction_id != parent_context.Camera ().ViewMatrixTransactionId ())
    {
      view_tm             = parent_context.Camera ().ViewMatrix ();
      view_transaction_id = parent_context.Camera ().ViewMatrixTransactionId ();

      frame_properties.SetProperty (view_matrix_property_index, parent_context.Camera ().ViewMatrix ());
    }

    if (proj_transaction_id != parent_context.Camera ().ProjectionMatrixTransactionId ())
    {
      proj_transaction_id = parent_context.Camera ().ProjectionMatrixTransactionId ();

      frame_properties.SetProperty (proj_matrix_property_index, parent_context.Camera ().ProjectionMatrix ());
    }
      
      //обновление свойств объект-кадр (для рендеринга спрайтов)

    frame_entity_properties.SetProperty (mv_matrix_property_index,  view_proj_tm);
    frame_entity_properties.SetProperty (mvp_matrix_property_index, view_proj_tm);
    frame_entity_properties.SetProperty (object_matrix_property_index, math::mat4f (1.0f));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicRenderer::ConfigureFrame");
    throw;
  }
}

void BasicRenderer::Draw (RenderingContext& context, size_t visual_models_count, VisualModel* const* model_ptr, void* user_data) const
{
  for (;visual_models_count--; model_ptr++)
  {
    VisualModel& model = **model_ptr;

    try
    {
      model.Draw (context, user_data);
    }
    catch (std::exception& e)
    {
      context.RenderManager ().Log ().Printf ("%s\n    at render::scene::server::BasicRenderer::Draw(RenderingContext&,size_t,VisualModel* const*,void*)", e.what ());
    }
    catch (...)
    {
      context.RenderManager ().Log ().Printf ("unknown exception\n    at render::scene::server::BasicRenderer::Draw(RenderingContext&,size_t,VisualModel* const*,void*)");
    }
  }
}

void BasicRenderer::Draw (RenderingContext& context, const stl::vector<VisualModel*>& models, void* user_data) const
{
  if (models.empty ())
    return;

  Draw (context, models.size (), &models [0], user_data);
}

void BasicRenderer::DrawVisualModels (RenderingContext& parent_context)
{
  try
  {
      //построение списка моделей на отрисовку

    TraverseResult& result = parent_context.TraverseResult ();
    
      //обновление визуализируемых объектов

    RenderingContext context (parent_context, frame);

    Draw (context, result.visual_models, 0);

      //добавление дочернего кадра

    parent_context.Frame ().AddFrame (frame);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicRenderer::DrawVisualModels");
    throw;
  }
}

void BasicRenderer::Draw (RenderingContext& parent_context)
{
  try
  {
      //настройка кадра

    ConfigureFrame (parent_context);

      //отрисовка моделей

    DrawVisualModels (parent_context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicRenderer::Draw(RenderingContext&)");
    throw;
  }
}

/*
    Обработчик отрисовки объектов
*/

void BasicRenderer::EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void*, manager::EntityDrawParams& out_params)
{
  VisualModel* model = reinterpret_cast<VisualModel*> (entity.UserData ());

  if (!model)
    return;

  ConfigureEntity (frame, entity, *model, out_params);
}

void BasicRenderer::ConfigureEntity (manager::Frame& frame, manager::Entity& entity, VisualModel& model, manager::EntityDrawParams& out_params)
{
  common::PropertyMap& properties = out_params.properties;
  const math::mat4f&   object_tm  = model.WorldMatrix ();

  out_params.mvp_matrix = view_proj_tm * object_tm;

  properties.SetProperty (mv_matrix_property_index,  view_tm * object_tm);
  properties.SetProperty (mvp_matrix_property_index, out_params.mvp_matrix);
  properties.SetProperty (object_matrix_property_index, object_tm);
}
