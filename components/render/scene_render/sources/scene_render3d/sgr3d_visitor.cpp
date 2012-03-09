#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
===================================================================================================
    CollectionVisitor
===================================================================================================
*/

/*
    Конструктор
*/

CollectionVisitor::CollectionVisitor (TraverseResult& in_result, size_t in_filter)
  : filter (in_filter)
  , result (in_result)
{
}

/*
    Диспетчеризация по типам
*/

void CollectionVisitor::Visit (Renderable& entity)
{
  if (!(filter & Collect_Renderables))
    return;
    
  result.renderables.push_back (&entity);
}

void CollectionVisitor::Visit (VisualModel& entity)
{
  if (!(filter & Collect_Renderables))
    return;
    
  result.renderables.push_back (&entity);
}

void CollectionVisitor::Visit (Light& entity)
{
  if (!(filter & Collect_Lights))
    return;
    
  result.lights.push_back (&entity);
}
