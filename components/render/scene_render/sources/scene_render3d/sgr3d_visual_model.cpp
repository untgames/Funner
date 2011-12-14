#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации визуализируемой модели
*/

struct VisualModel::Impl
{
  scene_graph::VisualModel& model;          //визуализируемая модель
  Entity                    entity;         //объект рендеринга, соответствующий модели
  size_t                    mesh_name_hash; //хэш имени мэша
  
///Конструктор
  Impl (Scene& scene, scene_graph::VisualModel& in_model)
    : model (in_model)
    , entity (scene.Manager ().CreateEntity ())
    , mesh_name_hash (~0u)
  {
  }
};

/*
    Конструктор / деструктор
*/

VisualModel::VisualModel (Scene& scene, scene_graph::VisualModel& entity)
  : Renderable (scene, entity)
{
  try  
  {    
    impl = new Impl (scene, entity);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::VisualModel::VisualModel");
    throw;
  }
}

VisualModel::~VisualModel ()
{
}

/*
    Обход
*/

void VisualModel::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    Отрисовка и обновление
*/

void VisualModel::UpdateCore ()
{
  if (impl->mesh_name_hash != impl->model.MeshNameHash ())
  {
    impl->entity.SetPrimitive (impl->model.MeshName ());
    
    impl->mesh_name_hash = impl->model.MeshNameHash ();
  }
}

void VisualModel::UpdateFrameCore (Frame& frame)
{
  frame.AddEntity (impl->entity);
}
