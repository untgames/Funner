#include <sg/light.h>
#include <xtl/visitor.h>
//#include <bv/sphere.h>

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float INFINITY = 1e9;  //если радиус/расстояние источника света превышает эту величину, устанавливаются бесконечные bv

/*
    Конструктор / деструктор
*/

PointLight::PointLight ()
{
}

PointLight::~PointLight ()
{
}

/*
    Создание источника света
*/

PointLight* PointLight::Create ()
{
  return new PointLight;
}

/*
   Рассчёт ограничивающего объёма
*/

void PointLight::ComputeBV ()
{
  if (Range () >= INFINITY)
    SetInfiniteBounds ();
  else
  {
//    sphere <float> bsphere (vec3f (0), Range ());
//    SetBoundBox (bsphere);
  }
}

/*
    Метод, вызываемый при посещении объекта
*/

void PointLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
