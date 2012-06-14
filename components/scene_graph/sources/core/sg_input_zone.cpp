#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации InputZone
*/

struct InputZone::Impl: public xtl::instance_counter<InputZone>
{
  ZoneDesc zone_desc; //описание спрайта
  
  Impl ()
  {
    zone_desc.position = vec3f (0.0f);  
    zone_desc.size     = vec2f (1.0f);
  }
};

/*
    Конструктор / деструктор
*/

InputZone::InputZone ()
  : impl (new Impl)
{
  SetBoundBox (bound_volumes::axis_aligned_box <float> (-0.5f, -0.5f, 0, 0.5f, 0.5f, 0));
}

InputZone::~InputZone ()
{
}

/*
    Создание спрайта
*/

InputZone::Pointer InputZone::Create ()
{
  return Pointer (new InputZone, false);
}

/*
    Метод, вызываемый при посещении объекта
*/

void InputZone::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    InputZoneModel::AcceptCore (visitor);
}

/*
    Реализация получения количества спрайтов и массива спрайтов
*/

size_t InputZone::ZoneDescsCountCore ()
{
  return 1;
}

const InputZoneModel::ZoneDesc* InputZone::ZoneDescsCore ()
{
  return &impl->zone_desc;
}
