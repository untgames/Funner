#include "shared.h"

using namespace scene_graph;

/*
    Конструктор / деструктор
*/

Scissor::Scissor ()
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (-0.5f, -0.5f, 0, 0.5f, 0.5f, 0));
}

Scissor::~Scissor ()
{
}

/*
    Создание 
*/

Scissor::Pointer Scissor::Create ()
{
  return Pointer (new Scissor, false);
}

/*
    Метод, вызываемый при посещении объекта
*/

void Scissor::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
