#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Конструктор
*/

Renderable::Renderable (Scene& scene, scene_graph::Entity& entity)
  : Node (scene, entity)
{
}

/*
    Обновление кадра
*/

void Renderable::UpdateFrame (Frame& frame)
{
  Update ();
  
  UpdateFrameCore (frame);
}

/*
    Обход
*/

void Renderable::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}
