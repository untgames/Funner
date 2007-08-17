#include "shared.h"

using namespace scene_graph::helpers;

/*
    Метод, вызываемый при посещении данного объекта
*/

void Helper::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);    
}
