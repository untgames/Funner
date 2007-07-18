#include "h/shared.h"

//Нет обработки setparam!!!!

/*
    ColladaTextureImpl
*/

ColladaTextureImpl::ColladaTextureImpl ()
{
  amount = 1;
}

/*
    ColladaTextureMapImpl
*/

ColladaTextureMapImpl::ColladaTextureMapImpl ()
{
  has_color = false;
}

/*
    ColladaTexture
*/

ColladaTexture::ColladaTexture (const ColladaTextureImpl* source)
  : ColladaEntity (source), impl (source)
  { }

const char* ColladaTexture::Image () const
{
  return impl->texture.c_str();
}

float ColladaTexture::Amount () const
{
  return impl->amount;
}

const math::mat4f& ColladaTexture::Transform () const
{
  return impl->transform;
}

/*
    ColladaEffect
*/

ColladaEffect::ColladaEffect (const ColladaEffectImpl* source)
  : ColladaEntity (source), impl (source)
  { }

ColladaShaderType ColladaEffect::ShaderType () const
{
  return impl->shader_type;
}

bool ColladaEffect::HasTexture (ColladaTextureMap map) const
{
  switch (map)
  {
    case COLLADA_MAP_DIFFUSE:
    case COLLADA_MAP_AMBIENT:
    case COLLADA_MAP_SPECULAR:
    case COLLADA_MAP_TRANSPARENT:
    case COLLADA_MAP_EMISSION:
    case COLLADA_MAP_REFLECTIVE:
    case COLLADA_MAP_BUMP:
      return !impl->shader.maps [map].texture.texture.empty ();
    default:
      RaiseInvalidArgument ("ColladaEffect::HasTexture","map",map);
      return false;
  }
}

ColladaTexture ColladaEffect::Texture (ColladaTextureMap map) const
{
  switch (map)
  {
    case COLLADA_MAP_DIFFUSE:
    case COLLADA_MAP_AMBIENT:
    case COLLADA_MAP_SPECULAR:
    case COLLADA_MAP_TRANSPARENT:
    case COLLADA_MAP_EMISSION:
    case COLLADA_MAP_REFLECTIVE:
    case COLLADA_MAP_BUMP:
      return ColladaWrappers::Create<ColladaTexture> (&impl->shader.maps [map].texture);
    default:
      RaiseInvalidArgument ("ColladaEffect::Texture","map",map);
      break;
  }
  
  static ColladaTextureImpl err_texture;
  
  return ColladaWrappers::Create <ColladaTexture> (&err_texture);
}

bool ColladaEffect::HasColor (ColladaTextureMap map) const
{
  switch (map)
  {
    case COLLADA_MAP_DIFFUSE:
    case COLLADA_MAP_AMBIENT:
    case COLLADA_MAP_SPECULAR:
    case COLLADA_MAP_TRANSPARENT:
    case COLLADA_MAP_EMISSION:
    case COLLADA_MAP_REFLECTIVE:
    case COLLADA_MAP_BUMP:
      return impl->shader.maps [map].has_color;
    default:
      RaiseInvalidArgument ("ColladaEffect::HasColor","map",map); 
      return false;
  }
}

const math::vec4f& ColladaEffect::MapColor (ColladaTextureMap map) const
{
  switch (map)
  {
    case COLLADA_MAP_DIFFUSE:
    case COLLADA_MAP_AMBIENT:
    case COLLADA_MAP_SPECULAR:
    case COLLADA_MAP_TRANSPARENT:
    case COLLADA_MAP_EMISSION:
    case COLLADA_MAP_REFLECTIVE:
    case COLLADA_MAP_BUMP:
      return impl->shader.maps [map].color;
    default:
      RaiseInvalidArgument ("ColladaEffect::MapColor","map",map);
      break;
  }

  static math::vec4f err_color;

  return err_color;
}

float ColladaEffect::Reflectivity () const
{
  return impl->shader.reflectivity;
}

float ColladaEffect::Transparency () const
{
  return impl->shader.transparency;
}

float ColladaEffect::RefractonIndex () const
{
  return impl->shader.index_of_refraction;
}

float ColladaEffect::Shininess () const
{
  return impl->shader.shininess;
}

/*
    ColladaImpl
*/

void ColladaImpl::parse_library_effects (Parser::Iterator p)
{
  if (!p->Present("effect"))
  {
    log->Error (p,"Uncorrect 'library_effects' tag. Must be at least one 'effect' sub-tag");
    return;
  }

  for (Parser::NamesakeIterator i=p->First ("effect");i;++i)
    parse_effect (i);
}

void ColladaImpl::parse_library_images (Parser::Iterator p)
{
  if (!p->Present("image"))
  {
    log->Error (p,"Uncorrect 'library_images' tag. Must be at least one 'image' sub-tag");
    return;
  }

  for (Parser::NamesakeIterator i=p->First ("image");i;++i)
  {
    images.resize (images.size() + 1);
    parse_image   (i,&images.back ());
  }
}

void ColladaImpl::parse_effect (Parser::Iterator p)
{
  effects.resize (effects.size () + 1);

  ColladaEffectImpl* new_effect = &effects.back ();

  new_effect->line = p->LineNumber ();
  
  p->Read ("id",new_effect->id,"No id");
  
  for (Parser::NamesakeIterator i=p->First ("image");i;++i)
  {
    new_effect->images.resize (images.size () + 1);
    parse_image               (i,&new_effect->images.back ());
  }

  parse_profile_COMMON (p->First ("profile_COMMON"),new_effect);
}

void ColladaImpl::parse_profile_COMMON (Parser::Iterator p, ColladaEffectImpl *destination)
{
  if (!p)
    return;

  if (!p->Present ("technique"))
  {
    log->Error (p,"Uncorrect 'profile_COMMON' tag. Must be at least one 'technique' sub-tag");
    return;
  }
  
    //чтение техники

  Parser::Iterator technique = p->First ("technique");    
  
    //чтение "экстра" параметров

  if (technique->Present("extra.technique.bump.texture"))
    parse_texture (technique->First("extra.technique.bump.texture"), &destination->shader.maps[COLLADA_MAP_BUMP].texture);

    //чтение картинок, связанных с данной техникой

  for (Parser::NamesakeIterator i=technique->First ("image");i;++i)
  {
    destination->images.resize (images.size () + 1);
    parse_image                (i,&destination->images.back ());
  }
  
    //чтение типа шейдера
  
  struct ShaderTypeName
  {
    const char*       tag;
    ColladaShaderType type;
  };
  
  static ShaderTypeName type_names [] = {
    {"constant",COLLADA_SHADER_CONSTANT},
    {"lambert",COLLADA_SHADER_LAMBERT},
    {"phong",COLLADA_SHADER_PHONG},
    {"blinn",COLLADA_SHADER_BLINN}
  };

  const size_t type_names_count = sizeof (type_names)/sizeof (ShaderTypeName);
  size_t       type_index       = 0;

  for (;type_index<type_names_count;type_index++)
    if (technique->Present (type_names [type_index].tag))
    {
      destination->shader_type = type_names [type_index].type;
      technique = technique->First (type_names [type_index].tag);
      break;
    }
    
  if (type_index == type_names_count)
  {
    log->Warning (technique, "Uncorrect 'technique' tag, no expected sub-tag (one of 'constant', 'lambert', 'phong' or 'blinn')");
    return;
  }
  
    //чтение базовых параметров техники

  technique->Read ("shininess.float.#text",destination->shader.shininess);
  technique->Read ("reflectivity.float.#text",destination->shader.reflectivity);
  technique->Read ("transparency.float.#text",destination->shader.transparency);
  technique->Read ("index_of_refraction.float.#text",destination->shader.index_of_refraction);
  
    //чтение текстурных карт
  
  struct TextureMapName
  {
    const char*       tag;
    ColladaTextureMap map;
  };
  
  static TextureMapName texmap_names [] = {
    {"emission",COLLADA_MAP_EMISSION},
    {"ambient",COLLADA_MAP_AMBIENT},
    {"diffuse",COLLADA_MAP_DIFFUSE},
    {"specular",COLLADA_MAP_SPECULAR},
    {"reflective",COLLADA_MAP_REFLECTIVE},
    {"transparent",COLLADA_MAP_TRANSPARENT}
  };
  
  const size_t texmap_names_count = sizeof (texmap_names)/sizeof (TextureMapName);

  for (size_t i=0;i<texmap_names_count;i++)
  {
    Parser::Iterator p = technique->First (texmap_names [i].tag);
    
    if (!p)
      continue;
      
    ColladaTextureMapImpl& texmap = destination->shader.maps [texmap_names [i].map];

    if (p->Present ("color"))
    {
      p->Read ("color.#text",texmap.color);
      texmap.has_color = true;        
    }
    
    if (p->Present ("texture"))
    {
      if (texmap.has_color)
      {
        log->Error (p->First ("texture"),"Texture map must have color or texture. Texture will be ignored");
        continue;
      }
      
      parse_texture (p->First ("texture"),&texmap.texture);
    }
  }
}

void ColladaImpl::parse_image (Parser::Iterator p, ColladaImageImpl *destination)
{
  p->Read ("id",destination->id,"No id");
  p->Read ("init_from.#text",destination->url);

  if (destination->url.empty ())
    log->Warning (p,"Empty 'init_from' tag");
}

void ColladaImpl::parse_texture (Parser::Iterator p, ColladaTextureImpl *destination)
{
  destination->line = p->LineNumber();

  p->Read ("texture",destination->texture);
  p->Read ("texcoord",destination->texcoord);  
  p->Read ("extra.technique.amount.#text",destination->amount,1.0f);
}
