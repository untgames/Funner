#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации InputZone
*/

struct InputZone::Impl: public xtl::instance_counter<InputZone>
{
  ZoneDesc zone_desc [NodeOrt_Num];   //описание спрайта
  size_t   plane_mask;                //маска включеных плоскостей ввода
  size_t   zones_count;               //количество зон
  
  Impl ()
  {
    plane_mask = 1 << NodeOrt_Z;

    UpdatePlanes ();
  }
  
  void UpdatePlanes ()
  {
    zones_count = 0;
    
    if (plane_mask & (1 << NodeOrt_X))
    {
      zone_desc [zones_count].position = vec3f (0.0f, -0.5f, -0.5f);
      zone_desc [zones_count].axis_x   = vec3f (0.0f, 1.0f, 0.0f);
      zone_desc [zones_count].axis_y   = vec3f (0.0f, 0.0f, 1.0f);

      zones_count++;
    }

    if (plane_mask & (1 << NodeOrt_Y))
    {
      zone_desc [zones_count].position = vec3f (-0.5f, 0.0f, -0.5f);
      zone_desc [zones_count].axis_x   = vec3f (0.0f, 0.0f, 1.0f);
      zone_desc [zones_count].axis_y   = vec3f (1.0f, 0.0f, 0.0f);

      zones_count++;
    }

    if (plane_mask & (1 << NodeOrt_Z))
    {    
      zone_desc [zones_count].position = vec3f (-0.5f, -0.5f, 0.0f);  
      zone_desc [zones_count].axis_x   = vec3f (1.0f, 0.0f, 0.0f);
      zone_desc [zones_count].axis_y   = vec3f (0.0f, 1.0f, 0.0f);        

      zones_count++;
    }
  }
};

/*
    Конструктор / деструктор
*/

InputZone::InputZone ()
  : impl (new Impl)
{
  DisableBoundsAutoUpdate ();

  SetBoundBox (bound_volumes::axis_aligned_box <float> (-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f));
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
  return impl->zones_count;
}

const InputZoneModel::ZoneDesc* InputZone::ZoneDescsCore ()
{
  return &impl->zone_desc [0];
}

/*
    Управление плоскостями зон ввода
*/

void InputZone::SetPlaneState (NodeOrt normal, bool state)
{
  switch (normal)
  {
    case NodeOrt_X:
    case NodeOrt_Y:
    case NodeOrt_Z:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::InputZone::SetPlaneState", "normal", normal);
  }
    
  size_t update_mask = 1 << normal;
  
  if (state)
  {
    if (impl->plane_mask & update_mask)
      return;
      
    impl->plane_mask |= update_mask;
  }
  else
  {
    if (!(impl->plane_mask & update_mask))
      return;
      
    impl->plane_mask &= ~update_mask;
  }

  impl->UpdatePlanes ();
  
  UpdateZoneDescsNotify ();
}

bool InputZone::IsPlaneEnabled (NodeOrt normal) const
{
  switch (normal)
  {
    case NodeOrt_X:
    case NodeOrt_Y:
    case NodeOrt_Z:
      return (impl->plane_mask & (int)(1 << normal)) != 0;
    default:
      return false;
  }
}
