#include "shared.h"

/*
    Разбор библиотеки эффектов
*/

void DaeParser::ParseLibraryEffects (Parser::Iterator iter)
{
  if (!iter->First ("effect"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_effects' node. Must be at least one 'effect' sub-tag");
    return;
  }
  
  for_each_child (*iter, "effect", xtl::bind (&DaeParser::ParseEffect, this, _1));
}

void DaeParser::ParseEffect (Parser::Iterator iter)
{
  const char* id = get<const char*> (*iter, "id");

    //создание эффекта

  Effect effect;

  effect.SetId (id);

    //разбор профилей эффекта

  common::ParseNode profile_common = iter->First ("profile_COMMON");

  if (profile_common)
    ParseEffectProfileCommon (profile_common, effect);

    //добавление эффекта в библиотеку

  model.Effects ().Insert (id, effect);
}

void DaeParser::ParseEffectProfileCommon (Parser::Iterator profile_iter, Effect& effect)
{
  if (!profile_iter->First ("technique"))
    raise_parser_exception (*profile_iter, "Incorrect 'profile_COMMON' tag. Node must have 'technique' sub-tag");

    //чтение техники

  Parser::Iterator technique_iter = profile_iter->First ("technique");

    //чтение экстра параметров

  if (Parser::Iterator bump_texture_iter = technique_iter->First ("extra.technique.bump.texture"))
    ParseTexture (bump_texture_iter, profile_iter, effect, TextureMap_Bump);

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
    if (technique_iter->First (shader_types [type_index].string))
    {
      effect.SetShaderType (shader_types [type_index].value);
      technique_iter = technique_iter->First (shader_types [type_index].string);
      break;
    }

  if (type_index == shader_types_count)
    raise_parser_exception (*technique_iter, "Uncorrect 'technique' tag, no expected sub-tag (one of 'constant', 'lambert', 'phong' or 'blinn')");
  
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
    if (common::ParseNode param_node = technique_iter->First (shader_params [i].string))
      effect.SetParam (shader_params [i].value, get<float> (param_node, ""));
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

    bool has_color = false;

    if (common::ParseNode param_node = texmap_iter->First ("color.#text"))
    {
      effect.SetMapColor (texmaps [i].value, get<vec4f> (param_node, ""));
      has_color = true;
    }
    
    Parser::Iterator texture_iter = texmap_iter->First ("texture");

    if (texture_iter)
    {
      if (has_color)
        raise_parser_exception (*texture_iter, "Texture map must have color or texture");

      ParseTexture (texture_iter, profile_iter, effect, texmaps [i].value);
    }      
  }
}

void DaeParser::ParseTexture (Parser::Iterator iter, Parser::Iterator profile_iter, Effect& effect, TextureMap map)
{
  Texture texture;

  const char *sampler_name  = get<const char*> (*iter, "texture", ""),
             *texcoord_name = get<const char*> (*iter, "texcoord", "");
             
  if (!*sampler_name)
    raise_parser_exception (*iter, "Node 'texture' not found");

  Parser::NamesakeIterator param_iter = profile_iter->First ("newparam");

  const char* surface_name = 0;

  for (; param_iter; ++param_iter)
    if (!strcmp (get<const char*> (*param_iter, "sid", ""), sampler_name))
    {
      surface_name = get<const char*> (*param_iter, "sampler2D.source.#text");

      break;
    }

  if (!param_iter)
    raise_parser_exception (*profile_iter, "There is no sampler2D with sid '%s'", sampler_name);

  param_iter = profile_iter->First ("newparam");    
  
  const char* image_name = 0;
  
  for (; param_iter; ++param_iter)
    if (!strcmp (get<const char*> (*param_iter, "sid", ""), surface_name))
    {
      image_name = get<const char*> (*param_iter, "surface.init_from.#text", (const char*)0);
      break;
    }  

  if (!image_name)
    raise_parser_exception (*profile_iter, "There is no surface with sid '%s'", surface_name);

  texture.SetImage (image_name);
             
  if (*texcoord_name)
    texture.SetTexcoordChannel (texcoord_name);             

  if (common::ParseNode param_node = iter->First ("extra.technique.amount.#text"))
    texture.SetAmount (get<float> (param_node, ""));

    //добавить чтение экстра-полей
    
  effect.SetTexture (map, texture);
}
