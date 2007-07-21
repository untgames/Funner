#include "shared.h"

/*
    Разбор библиотеки эффектов
*/

void DaeParser::ParseLibraryEffects (Parser::Iterator iter)
{
  if (!test (iter, "effect"))
  {
    LogError (iter, "Incorrect 'library_effects' tag. Must be at least one 'effect' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "effect", xtl::bind (&DaeParser::ParseEffect, this, _1));
}

void DaeParser::ParseEffect (Parser::Iterator iter)
{
  LogScope scope (iter, *this);

  const char* id = get<const char*> (iter, "id");

  if (!id)
  {
    LogError (iter, "Incorrect 'id' attribute");
    return;
  }
  
    //создание эффекта
    
  Effect& effect = model.Effects ().Create (id);  

    //разбор профилей эффекта

  parse_if (iter, "profile_COMMON", xtl::bind (&DaeParser::ParseEffectProfileCommon, this, _1, ref (effect)));
}

void DaeParser::ParseEffectProfileCommon (Parser::Iterator profile_iter, Effect& effect)
{
  if (!test (profile_iter, "technique"))
  {
    LogError (profile_iter, "Incorrect 'profile_COMMON' tag. Node must have 'technique' sub-tag");
    return;
  }

    //чтение техники

  Parser::Iterator technique_iter = profile_iter->First ("technique");    
  
  LogScope technique_scope (technique_iter, *this);
  
    //чтение экстра параметров
    
  if (Parser::Iterator bump_texture_iter = technique_iter->First ("extra.technique.bump.texture"))
    if (!ParseTexture (bump_texture_iter, profile_iter, effect.CreateTexture (TextureMap_Bump)))
      effect.RemoveTexture (TextureMap_Bump);

    //чтение типа шейдера

  static String2Value<ShaderType> shader_types [] = {
    {"constant", ShaderType_Constant},
    {"lambert",  ShaderType_Lambert},
    {"phong",    ShaderType_Phong},
    {"blinn",    ShaderType_Blinn}
  };

  const size_t shader_types_count = sizeof (shader_types) / sizeof (*shader_types);
  
  size_t type_index = 0;

  for (; type_index<shader_types_count; type_index++)
    if (test (technique_iter, shader_types [type_index].string))
    {
      effect.SetShaderType (shader_types [type_index].value);
      technique_iter = technique_iter->First (shader_types [type_index].string);
      break;
    }
    
  if (type_index == shader_types_count)
  {
    LogError (technique_iter, "Uncorrect 'technique' tag, no expected sub-tag (one of 'constant', 'lambert', 'phong' or 'blinn')");
    return;
  }
  
    //чтение базовых параметров техники
    
  static String2Value<EffectParam> shader_params [] = {
    {"shininess.float.#text",           EffectParam_Shininess},
    {"reflectivity.float.#text",        EffectParam_Reflectivity},
    {"transparency.float.#text",        EffectParam_Transparency},
    {"index_of_refraction.float.#text", EffectParam_RefractionIndex}
  };
  
  const size_t shader_params_count = sizeof (shader_params) / sizeof (*shader_params);

  for (size_t i=0; i<shader_params_count; i++)
  {
    float value = 0.0f;
    
    if (CheckedRead (technique_iter, shader_params [i].string, value))
      effect.SetParam (shader_params [i].value, value);    
  }

    //чтение текстурных карт

  static String2Value<TextureMap> texmaps [] = {
    {"diffuse", TextureMap_Diffuse},
    {"ambient", TextureMap_Ambient},
    {"specular", TextureMap_Specular},
    {"transparent", TextureMap_Transparent},
    {"emission", TextureMap_Emission},
    {"reflective", TextureMap_Reflective},
  };
  
  const size_t texmaps_count = sizeof (texmaps) / sizeof (*texmaps);

  for (size_t i=0; i<texmaps_count; i++)
  {
    Parser::Iterator texmap_iter = technique_iter->First (texmaps [i].string);

    if (!texmap_iter)
      continue;

    bool  has_color = false;
    vec4f color;

    if (CheckedRead (texmap_iter, "color.#text", color))
    {
      effect.SetMapColor (texmaps [i].value, color);
      has_color = true;
    }
    
    Parser::Iterator texture_iter = texmap_iter->First ("texture");

    if (texture_iter)
    {
      if (has_color)
      {
        LogError (texture_iter, "Texture map must have color or texture. Texture will be ignored");
        continue;
      }
      
      if (!ParseTexture (texture_iter, profile_iter, effect.CreateTexture (texmaps [i].value)))
        effect.RemoveTexture (texmaps [i].value);
    }      
  }
}

bool DaeParser::ParseTexture (Parser::Iterator iter, Parser::Iterator profile_iter, Texture& texture)
{
  LogScope scope (iter, *this);

  const char *sampler_name  = get<const char*> (iter, "texture"),
             *texcoord_name = get<const char*> (iter, "texcoord");
             
  if (!sampler_name)
  {
    LogError (iter, "Incorrect 'texture' sub-tag");
    return false;    
  }
  
  Parser::NamesakeIterator param_iter = profile_iter->First ("newparam");
  
  const char* surface_name = 0;
  
  for (; param_iter; ++param_iter)
    if (test (param_iter, "sid", sampler_name))
    {
      surface_name = get<const char*> (param_iter, "sampler2D.source.#text");
      
      if (!surface_name)
      {
        LogError (param_iter, "Incorrect 'sampler2D.source' sub-tag");
        return false;
      }

      break;
    }
    
  if (!param_iter)
  {
    LogError (profile_iter, "There is no sampler2D with sid '%s'", sampler_name);
    return false;
  }
    
  param_iter = profile_iter->First ("newparam");    
  
  const char* image_name = 0;
  
  for (; param_iter; ++param_iter)
    if (test (param_iter, "sid", surface_name))
    {
      image_name = get<const char*> (param_iter, "surface.init_from.#text");
      
      if (!image_name)
      {
        LogError (param_iter, "Incorrect 'surface.init_from' sub-tag");
        return false;
      }

      break;
    }  
      
  if (!image_name)
  {
    LogError (profile_iter, "There is no surface with sid '%s'", surface_name);
    return false;
  }
  
  texture.SetImage (image_name);
             
  if (texcoord_name)
    texture.SetTexcoordChannel (texcoord_name);             
             
  float amount = 1.0f;
             
  if (CheckedRead (iter, "extra.technique.amount.#text", amount))
    texture.SetAmount (amount);
    
    //добавить чтение экстра-полей
    
  return true;
}
