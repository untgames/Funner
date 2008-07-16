#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

/*
    Конструктор / деструктор
*/

Renderable::Renderable (scene_graph::Entity* entity)
  : on_update_connection (entity->RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&Renderable::UpdateNotify, this))),
    need_update (true)
{
}

/*
    Оповещение об обновлении объекта
*/

void Renderable::UpdateNotify ()
{
  need_update = true;
}

/*
    Рисование
*/

void Renderable::Draw (IFrame& frame)
{
  if (need_update)
  {
    Update ();
    
    need_update = false;
  }
  
  DrawCore (frame);
}
