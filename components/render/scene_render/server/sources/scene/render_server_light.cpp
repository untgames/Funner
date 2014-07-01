#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации отображаемой модели
*/

namespace
{

const float DEFAULT_LIGHT_RANGE  = 1e9;  //расстояние действия источника света по умолчанию
const float DEFAULT_LIGHT_RADIUS = 1e9;  //радиус действия источника света по умолчанию

}

struct Light::Impl
{
  interchange::NodeType node_type; //тип источника
  LightParams           params;    //параметры источника

/// Конструктор
  Impl (interchange::NodeType in_node_type)
    : node_type (in_node_type)
  {
    params.color       = math::vec3f (1.0f);
    params.intensity   = 1.0f;
    params.attenuation = 0.0f;
    params.range       = DEFAULT_LIGHT_RANGE;
    params.angle       = math::anglef ();
    params.exponent    = 0.0f;
    params.radius      = DEFAULT_LIGHT_RADIUS;

    switch (node_type)
    {
      case interchange::NodeType_PointLight:
      case interchange::NodeType_SpotLight:
      case interchange::NodeType_DirectLight:
        break;
      default:
        throw xtl::make_argument_exception ("render::scene::server::Light::Light", "node_type", node_type);
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Light::Light (interchange::NodeType node_type)
  : impl (new Impl (node_type))
{
}

Light::~Light ()
{
}

/*
    Тип источника
*/

interchange::NodeType Light::Type () const
{
  return impl->node_type;
}

/*
    Параметры источника
*/

void Light::SetParams (const LightParams& params)
{
  impl->params = params;
}

const LightParams& Light::Params () const
{
  return impl->params;
}

/*
    Обход
*/

void Light::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
