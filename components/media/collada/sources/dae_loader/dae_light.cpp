#include "shared.h"

using namespace media::collada;

/*
    Разбор библиотеки источников света
*/

void DaeParser::ParseLibraryLights (Parser::Iterator iter)
{
  if (!test (iter, "light"))
  {
    LogError (iter, "Incorrect 'library_lights' node. Must be at least one 'light' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "light", xtl::bind (&DaeParser::ParseLight, this, _1));
}

/*
    Разбор источника света
*/

void DaeParser::ParseLight (Parser::Iterator iter)
{
  const char* id = get<const char*> (iter, "id");
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
  LogScope scope (iter, *this);

  iter = iter->First ("technique_common");
  
  if (!iter)
    return;

  if (iter->NextNamesake ())
    LogWarning (iter->NextNamesake (), "Second 'technique_common' tag detected");
    
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
    if (test (iter, light_types [i].string))
    {
      type = light_types [i].value;      
      iter = iter->First (light_types [i].string);
      break;
    }
    
  if (i == light_types_count)
  {
    LogError (iter, "Incorrect 'technique_common' tag. No light type sub-tag (one of 'ambient', 'directional', 'point' or 'spot')");
    return;
  }
  
    //создание источника света
    
  Light light;
  
  light.SetId (id);
  
    //установка типа источника
    
  light.SetType (type);
  
    //чтение цвета
  
  vec3f color = 0.0f;
  
  if (CheckedRead (iter, "color.#text", color))  
    light.SetColor (color);
  
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
    if (test (iter, light_params [i].string))
    {
      float value = 0.0f;
      
      if (CheckedRead (iter, light_params [i].string, value))
        light.SetParam (light_params [i].value, value);
    }
    
    //добавление источника в библиотеку
  
  model.Lights ().Insert (id, light);
}
