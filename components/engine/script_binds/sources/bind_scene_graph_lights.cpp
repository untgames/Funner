#include "scene_graph.h"

using namespace scene_graph;

template class engine::decl_sg_cast<Light,       VisualModel>;
template class engine::decl_sg_cast<Light,       Entity>;
template class engine::decl_sg_cast<Light,       Node>;
template class engine::decl_sg_cast<DirectLight, Light>;
template class engine::decl_sg_cast<DirectLight, Entity>;
template class engine::decl_sg_cast<DirectLight, VisualModel>;
template class engine::decl_sg_cast<DirectLight, Node>;
template class engine::decl_sg_cast<PointLight,  Light>;
template class engine::decl_sg_cast<PointLight,  Entity>;
template class engine::decl_sg_cast<PointLight,  VisualModel>;
template class engine::decl_sg_cast<PointLight,  Node>;
template class engine::decl_sg_cast<SpotLight,   Light>;
template class engine::decl_sg_cast<SpotLight,   Entity>;
template class engine::decl_sg_cast<SpotLight,   VisualModel>;
template class engine::decl_sg_cast<SpotLight,   Node>;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Создание источников света
*/

DirectLight::Pointer create_direct_light ()
{
  return DirectLight::Create ();
}

SpotLight::Pointer create_spot_light ()
{
  return SpotLight::Create ();
}

PointLight::Pointer create_point_light ()
{
  return PointLight::Create ();
}

/*
    Регистрация библиотек работы с источниками света
*/

void bind_direct_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_DIRECT_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_direct_light));

    //регистрация операций

  lib.Register ("set_Radius", make_invoker (&DirectLight::SetRadius));
  lib.Register ("get_Radius", make_invoker (&DirectLight::Radius));

    //регистрация типов данных

  environment.RegisterType<DirectLight> (SCENE_DIRECT_LIGHT_LIBRARY);
}

void bind_spot_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SPOT_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_spot_light));

    //регистрация операций

  lib.Register ("set_Angle",    make_invoker<void (SpotLight&, math::anglef)> (&SpotLight::SetAngle));
  lib.Register ("set_Exponent", make_invoker (&SpotLight::SetExponent));
  lib.Register ("get_Angle",    make_invoker<math::anglef (SpotLight&)> (&SpotLight::Angle));
  lib.Register ("get_Exponent", make_invoker (&SpotLight::Exponent));

    //регистрация типов данных

  environment.RegisterType<SpotLight> (SCENE_SPOT_LIGHT_LIBRARY);
}

void bind_point_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_POINT_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_point_light));

    //регистрация типов данных

  environment.RegisterType<PointLight> (SCENE_POINT_LIGHT_LIBRARY);
}

void bind_light_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистрация операций

  lib.Register ("set_LightColor",  make_invoker (&Light::SetLightColor));
  lib.Register ("set_Attenuation", make_invoker (&Light::SetAttenuation));
  lib.Register ("set_Range",       make_invoker (&Light::SetRange));
  lib.Register ("get_LightColor",  make_invoker (&Light::LightColor));
  lib.Register ("get_Attenuation", make_invoker (&Light::Attenuation));
  lib.Register ("get_Range",       make_invoker (&Light::Range));

    //регистрация типов данных

  environment.RegisterType<Light> (SCENE_LIGHT_LIBRARY);
}

}

}
