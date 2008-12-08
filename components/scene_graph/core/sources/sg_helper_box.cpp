#include "shared.h"

using namespace scene_graph::helpers;
using namespace bound_volumes;

/*
    Конструктор
*/

Box::Box ()
{
    //по умолчанию создаётся параллелепиппед с рёбрами единичной длины
    
  SetBoundBox (aaboxf (-1.0f, 1.0f));
}

/*
    Создание ()
*/

Box::Pointer Box::Create ()
{
  return Pointer (new Box, false);
}

/*
    Метод, вызываемый при посещении данного объекта
*/

void Box::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Helper::AcceptCore (visitor);
}
