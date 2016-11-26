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

/*
    Связывание свойств
*/

namespace
{

void set_box_min (Box* box, const math::vec3f& v)
{
  box->SetBoundBox (aaboxf (v, box->BoundBox ().maximum ()));
}

void set_box_max (Box* box, const math::vec3f& v)
{
  box->SetBoundBox (aaboxf (box->BoundBox ().minimum (), v));
}

math::vec3f get_box_min (Box* box)
{
  return box->BoundBox ().minimum ();
}

math::vec3f get_box_max (Box* box)
{
  return box->BoundBox ().maximum ();
}

}

void Box::BindProperties (common::PropertyBindingMap& bindings)
{
  Helper::BindProperties (bindings);

  bindings.AddProperty ("MinBound", xtl::bind (&get_box_min, this), xtl::bind (&set_box_min, this, _1));
  bindings.AddProperty ("MaxBound", xtl::bind (&get_box_max, this), xtl::bind (&set_box_max, this, _1));
}
