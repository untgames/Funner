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

CollectionVisitor::CollectionVisitor (TraverseResults& in_results, size_t in_filter)
  : results (in_results)
  , filter (in_filter)
{
}

/*
    Диспетчеризация по типам
*/

void CollectionVisitor::Visit (Renderable& entity)
{
  if (!(filter & Collect_Renderables))
    return;
    
  results.renderables.push_back (&entity);
}

void CollectionVisitor::Visit (VisualModel& entity)
{
  if (!(filter & Collect_Renderables))
    return;
    
  results.renderables.push_back (&entity);
}

void CollectionVisitor::Visit (Light& entity)
{
  if (!(filter & Collect_Lights))
    return;
    
  results.lights.push_back (&entity);
}
