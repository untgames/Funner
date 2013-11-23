#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

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

void CollectionVisitor::Visit (VisualModel& entity)
{
  if (!(filter & Collect_VisualModels))
    return;
    
  result.visual_models.push_back (&entity);
}

void CollectionVisitor::Visit (Light& entity)
{
  if (!(filter & Collect_Lights))
    return;
    
  result.lights.push_back (&entity);
}
