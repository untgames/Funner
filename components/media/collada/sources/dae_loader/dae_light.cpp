#include "shared.h"

using namespace media::collada;

/*
    Разбор библиотеки источников света
*/

void DaeParser::ParseLibraryLights (Parser::Iterator iter)
{
  if (!iter->First ("light"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_lights' node. Must be at least one 'light' sub-tag");
    return;
  }

  for_each_child (*iter, "light", xtl::bind (&DaeParser::ParseLight, this, _1));
}

/*
    Разбор источника света
*/

void DaeParser::ParseLight (Parser::Iterator iter)
{
  const char* id = get<const char*> (*iter, "id");
   
  iter = iter->First ("technique_common");

  if (!iter)
    return;

  if (iter->NextNamesake ())
    raise_parser_exception (iter->NextNamesake (), "Only one 'technique_common' tag allowed");

    //чтение типа источника света

  static String2Value<LightType> light_types [] = {
    {"ambient",     LightType_Ambient},
    {"directional", LightType_Direct},
    {"point",       LightType_Point},
    {"spot",        LightType_Spot}
  };    

  static const size_t light_types_count = sizeof (light_types) / sizeof (*light_types);

  size_t i;
  LightType type = LightType_Point;
    
  for (i=0; i<light_types_count; i++)
    if (common::ParseNode param_node = iter->First (light_types [i].string))
    {
      type = light_types [i].value;
      iter = param_node;
      break;
    }

  if (i == light_types_count)
    raise_parser_exception (*iter, "Incorrect 'technique_common' tag. No light type sub-tag (one of 'ambient', 'directional', 'point' or 'spot')");
  
    //создание источника света
    
  Light light;
  
  light.SetId (id);
  
    //установка типа источника
    
  light.SetType (type);
  
    //чтение цвета
  
  if (common::ParseNode param_node = iter->First ("color.#text"))
    light.SetColor (get<vec3f> (param_node, ""));
  
    //чтение параметров источника света
    
  static String2Value<LightParam> light_params [] = {
    {"constant_attenuation.#text",  LightParam_AttenuationConstant},
    {"linear_attenuation.#text",    LightParam_AttenuationLinear},
    {"quadratic_attenuation.#text", LightParam_AttenuationQuadratic},
    {"falloff_angle.#text",         LightParam_FalloffAngle},
    {"falloff_exponent.#text",      LightParam_FalloffExponent}
  };
  
  static const size_t light_params_num = sizeof (light_params) / sizeof (*light_params);

  for (i=0; i<light_params_num; i++)
    if (common::ParseNode param_node = iter->First (light_params [i].string))
      light.SetParam (light_params [i].value, get<float> (param_node, ""));
    
    //добавление источника в библиотеку
  
  model.Lights ().Insert (id, light);
}
