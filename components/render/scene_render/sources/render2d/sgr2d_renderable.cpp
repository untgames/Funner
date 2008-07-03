#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

/*
    Конструктор / деструктор
*/

Renderable::Renderable (scene_graph::Entity* entity)
  : on_update_connection (entity->RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&Renderable::UpdateNotify, this)))
{
}
