#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации визуализируемой модели
*/

struct VisualModel::Impl
{
  scene_graph::VisualModel& model; //визуализируемая модель
  
///Конструктор
  Impl (scene_graph::VisualModel& in_model)
    : model (in_model)
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
    impl = new Impl (entity);
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

void VisualModel::UpdateCore (Frame& frame)
{
}

void VisualModel::UpdateFrameCore (Frame& frame)
{
}
