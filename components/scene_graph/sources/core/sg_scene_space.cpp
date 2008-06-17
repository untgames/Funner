#include "shared.h"

using namespace scene_graph;

/*
    Конструктор / деструктор
*/

SceneSpace::SceneSpace ()
{
  first_object  = 0;
  objects_count = 0;
}

SceneSpace::~SceneSpace ()
{
  while (first_object)
    first_object->BindToSpace (0);
}
