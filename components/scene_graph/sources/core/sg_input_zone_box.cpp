#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации InputZoneBox
*/

struct InputZoneBox::Impl: public xtl::instance_counter<InputZoneBox>
{
  ZoneDesc zone_desc [6]; //описания зон ввода
  
  Impl ()
  {
    zone_desc [0].position = vec3f (-0.5f, -0.5f, -0.5f);
    zone_desc [0].axis_x   = vec3f (0.0f, 1.0f, 0.0f);
    zone_desc [0].axis_y   = vec3f (0.0f, 0.0f, 1.0f);

    zone_desc [1].position = vec3f (-0.5f, -0.5f, -0.5f);
    zone_desc [1].axis_x   = vec3f (0.0f, 0.0f, 1.0f);
    zone_desc [1].axis_y   = vec3f (1.0f, 0.0f, 0.0f);

    zone_desc [2].position = vec3f (-0.5f, -0.5f, -0.5f);
    zone_desc [2].axis_x   = vec3f (1.0f, 0.0f, 0.0f);
    zone_desc [2].axis_y   = vec3f (0.0f, 1.0f, 0.0f);

    zone_desc [3].position = vec3f (0.5f, -0.5f, -0.5f);
    zone_desc [3].axis_x   = vec3f (0.0f, 1.0f, 0.0f);
    zone_desc [3].axis_y   = vec3f (0.0f, 0.0f, 1.0f);

    zone_desc [4].position = vec3f (-0.5f, 0.5f, -0.5f);
    zone_desc [4].axis_x   = vec3f (0.0f, 0.0f, 1.0f);
    zone_desc [4].axis_y   = vec3f (1.0f, 0.0f, 0.0f);

    zone_desc [5].position = vec3f (-0.5f, -0.5f, 0.5f);
    zone_desc [5].axis_x   = vec3f (1.0f, 0.0f, 0.0f);
    zone_desc [5].axis_y   = vec3f (0.0f, 1.0f, 0.0f);
  }
};

/*
    Конструктор / деструктор
*/

InputZoneBox::InputZoneBox ()
  : impl (new Impl)
{
  DisableBoundsAutoUpdate ();

  SetBoundBox (bound_volumes::axis_aligned_box <float> (-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f));
}

InputZoneBox::~InputZoneBox ()
{
}

/*
    Создание спрайта
*/

InputZoneBox::Pointer InputZoneBox::Create ()
{
  return Pointer (new InputZoneBox, false);
}

/*
    Метод, вызываемый при посещении объекта
*/

void InputZoneBox::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    InputZoneModel::AcceptCore (visitor);
}

/*
    Реализация получения количества спрайтов и массива спрайтов
*/

size_t InputZoneBox::ZoneDescsCountCore ()
{
  return sizeof (impl->zone_desc) / sizeof (*impl->zone_desc);
}

const InputZoneModel::ZoneDesc* InputZoneBox::ZoneDescsCore ()
{
  return &impl->zone_desc [0];
}
