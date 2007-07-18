#include "h/shared.h"

ColladaLight::ColladaLight (const ColladaLightImpl* source)
  : ColladaEntity(source), impl (source)
  { }

ColladaLightType ColladaLight::LightType () const
{
  return impl->type;
}

const math::vec3f& ColladaLight::Color ()
{
  return impl->color;
}

float ColladaLight::AttenuationConstant () const
{
  return impl->constant_attenuation;
}

float ColladaLight::AttenuationLinear () const
{
  return impl->linear_attenuation;
}

float ColladaLight::AttenuationQuadratic () const
{
  return impl->quadratic_attenuation;
}

float ColladaLight::FalloffAngle () const
{
  return impl->falloff_angle;
}

float ColladaLight::FalloffExponent () const
{
  return impl->falloff_exponent;
}

void ColladaImpl::parse_library_lights (Parser::Iterator p)
{
  if(!p->Present("light"))
  {
    log->Error(p, "Uncorrect 'library_lights' tag. Must be at least one 'light' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("light"); i; i++)
    parse_light (i);
}

void ColladaImpl::parse_light (Parser::Iterator p)
{
  Parser::NamesakeIterator i = p->First("technique_common");

  if (i)
  {
    lights.resize(lights.size() + 1);
    p->Read ("id", lights.back().id, "No id");
    lights.back().line = p->LineNumber ();
    parse_technique_common (i, &lights.back());

    if (++i)
      log->Error(p, "Detected more than one child 'technique_common'.");
  }
}

void ColladaImpl::parse_technique_common (Parser::Iterator p, ColladaLightImpl *destination)
{
  if(p->Present("ambient"))
  {
    p = p->First("ambient");
    destination->type = COLLADA_LIGHT_AMBIENT;
  }
  else if (p->Present("directional")) 
  {
    p = p->First("directional");
    destination->type = COLLADA_LIGHT_DIRECT;
  }
  else if (p->Present("point"))
  {
    p = p->First("point");
    destination->type = COLLADA_LIGHT_POINT;
  }              
  else if (p->Present("spot"))
  {
    p = p->First("spot");
    destination->type = COLLADA_LIGHT_SPOT;
  }
  else 
  {
    log->Error(p, "Uncorrect 'technique_common' tag, no expected sub-tag (one of 'ambient', 'directional', 'point' or 'spot')");
    return;
  }

  destination->color = 0;
  destination->constant_attenuation = destination->linear_attenuation = destination->quadratic_attenuation = 
                                                                        destination->falloff_angle = destination->falloff_exponent = 0;

  if (p->Present("color"))  
    p->First("color")->Read ("#text", destination->color);
  if (p->Present("constant_attenuation"))  
    p->First("constant_attenuation")->Read ("#text", destination->constant_attenuation);
  if (p->Present("linear_attenuation"))  
    p->First("linear_attenuation")->Read ("#text", destination->linear_attenuation);
  if (p->Present("quadratic_attenuation"))  
    p->First("quadratic_attenuation")->Read ("#text", destination->quadratic_attenuation);
  if (p->Present("falloff_angle"))  
    p->First("falloff_angle")->Read ("#text", destination->falloff_angle);
  if (p->Present("falloff_exponent"))  
    p->First("falloff_exponent")->Read ("#text", destination->falloff_exponent);
}

