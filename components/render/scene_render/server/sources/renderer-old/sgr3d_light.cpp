#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации источника света
*/

namespace
{

struct LightCommonData
{
  scene_graph::Light& light;       //ссылка на исходный источник света
  math::vec3f         light_color; //цвет
  float               intensity;   //интенсивность
  math::vec3f         attenuation; //затенение
  float               range;       //расстояние действия
  
///Конструктор
  LightCommonData (scene_graph::Light& in_light)
    : light (in_light)
    , light_color (in_light.LightColor ())
    , intensity (in_light.Intensity ())
    , attenuation (in_light.Attenuation ())    
    , range (in_light.Range ())
  {
  }
};

}

struct Light::Impl: public LightCommonData
{
  LightType           type;        //тип источника света
  float               exponent;    //экспонента рассеивания света (для spot-light)
  math::anglef        angle;       //угол действия (для spot-light)
  float               radius;      //радиус действия (для direct-light)
  
///Конструкторы
  Impl (scene_graph::PointLight& light)
    : LightCommonData (light)
    , type (LightType_Point)
    , exponent ()
    , angle ()
    , radius ()
  {
  }
  
  Impl (scene_graph::DirectLight& light)
    : LightCommonData (light)
    , type (LightType_Direct)
    , exponent ()
    , angle ()
    , radius (light.Radius ())
  {
  }
  
  Impl (scene_graph::SpotLight& light)
    : LightCommonData (light)
    , type (LightType_Spot)
    , exponent (light.Exponent ())
    , angle (light.Angle ())
    , radius ()
  {
  }  
};

/*
    Конструктор / деструктор
*/

Light::Light (Scene& scene, scene_graph::PointLight& light)
  : Node (scene, light)
  , impl (new Impl (light))
{
}

Light::Light (Scene& scene, scene_graph::DirectLight& light)
  : Node (scene, light)
  , impl (new Impl (light))
{
}

Light::Light (Scene& scene, scene_graph::SpotLight& light)
  : Node (scene, light)
  , impl (new Impl (light))
{
}

Light::~Light ()
{
}

/*
    Тип источника света
*/

LightType Light::Type ()
{
  return impl->type;
}

/*
    Параметры источника
*/

const math::vec3f& Light::LightColor ()
{
  return impl->light_color;
}

float Light::Intensity ()
{
  return impl->intensity;
}

const math::vec3f& Light::Attenuation ()
{
  return impl->attenuation;
}

float Light::Range ()
{
  return impl->range;
}

float Light::Radius ()
{
  return impl->radius;
}

const math::anglef& Light::Angle ()
{
  return impl->angle;
}

float Light::Exponent ()
{
  return impl->exponent;
}

/*
    Обновление состояния
*/

void Light::UpdateCore ()
{
  try
  {
    impl->light_color = impl->light.LightColor ();
    impl->intensity   = impl->light.Intensity ();
    impl->attenuation = impl->light.Attenuation ();
    impl->range       = impl->light.Range ();
    
    switch (impl->type)
    {
      case LightType_Point:
        break;
      case LightType_Direct:
        impl->radius = static_cast<scene_graph::DirectLight&> (impl->light).Radius ();
        break;
      case LightType_Spot:
      {
        scene_graph::SpotLight& light = static_cast<scene_graph::SpotLight&> (impl->light);
        
        impl->angle    = light.Angle ();
        impl->exponent = light.Exponent ();

        break;        
      }
      default:
        break;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Light::UpdateCore");
    throw;
  }
}

/*
    Обход
*/

void Light::VisitCore (IVisitor& visitor)
{
  visitor.Visit (*this);
}
