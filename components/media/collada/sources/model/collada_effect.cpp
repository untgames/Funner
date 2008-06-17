#include "shared.h"

using namespace media::collada;
using namespace media;
using namespace common;

/*
    Реализация эффекта
*/

struct Map
{
  Texture     texture; //текстура
  math::vec4f color;   //цвет карты
  
  Map () {}
};

struct Effect::Impl: public xtl::reference_counter
{
  media::collada::ShaderType  shader_type;                     //тип шейдера
  Map                         maps [TextureMap_Num];           //карты
  float                       shader_params [EffectParam_Num]; //параметры шейдинга
  stl::string                 id;                              //идентификатор эффекта
  
  Impl () : shader_type (ShaderType_Default)
  {
    for (size_t i=0; i<EffectParam_Num; i++)
      shader_params [i] = 0.0f;
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

Effect::Effect ()
  : impl (new Impl, false)
  {}

Effect::Effect (Impl* in_impl)
  : impl (in_impl, false)
  {}

Effect::Effect (const Effect& effect)
  : impl (effect.impl)
  {}

Effect::~Effect ()
{
}

Effect& Effect::operator = (const Effect& effect)
{
  impl = effect.impl;
  
  return *this;
}

/*
    Создание копии
*/

Effect Effect::Clone () const
{
  return Effect (new Impl (*impl));
}

/*
    Идентификатор эффекта
*/

const char* Effect::Id () const
{
  return impl->id.c_str ();
}

void Effect::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::collada::Effect::SetId", "id");
    
  impl->id = id;
}

/*
    Тип шейдера
*/

ShaderType Effect::ShaderType () const
{
  return impl->shader_type;
}

void Effect::SetShaderType (media::collada::ShaderType type)
{
  switch (type)
  {
    case ShaderType_Constant:
    case ShaderType_Lambert:
    case ShaderType_Phong:
    case ShaderType_Blinn:
      impl->shader_type = type;
      break;
    default:
      throw xtl::make_argument_exception ("media::collada::Effect::SetShaderType", "type", type);
      break;
  }
}
  
/*
    Работа с текстурными картами
*/

const Texture& Effect::Texture (TextureMap map) const
{
  if (map < 0 || map >= TextureMap_Num)
    throw xtl::make_argument_exception ("media::collada::Effect::Texture", "map", map);
    
  return impl->maps [map].texture;
}

Texture& Effect::Texture (TextureMap map)
{
  return const_cast<media::collada::Texture&> (const_cast<const Effect&> (*this).Texture (map));
}

void Effect::SetTexture (TextureMap map, media::collada::Texture& texture)
{
  if (map < 0 || map >= TextureMap_Num)
    throw xtl::make_argument_exception ("media::collada::Effect::SetTexture", "map", map);
    
  impl->maps [map].texture = texture;
}

bool Effect::HasTexture (TextureMap map) const
{
  if (map < 0 || map >= TextureMap_Num)
    return false;
    
  return *impl->maps [map].texture.Image () != '\0';
}

/*
    Работа с цветом карт
*/

const math::vec4f& Effect::MapColor (TextureMap map) const
{
  if (map < 0 || map >= TextureMap_Num)
    throw xtl::make_argument_exception ("media::collada::Effect::MapColor", "map", map);
    
  return impl->maps [map].color;
}

void Effect::SetMapColor (TextureMap map, const math::vec4f& color)
{
  if (map < 0 || map >= TextureMap_Num)
    throw xtl::make_argument_exception ("Effect::SetMapColor", "map", map);
    
  impl->maps [map].color = color;
}

/*
    Параметры шейдинга
*/

float Effect::Param (EffectParam param) const
{
  if (param < 0 || param >= EffectParam_Num)
    throw xtl::make_argument_exception ("media::collada::Effect::Param", "param", param);
    
  return impl->shader_params [param];
}

void Effect::SetParam (EffectParam param, float value)
{
  if (param < 0 || param >= EffectParam_Num)
    throw xtl::make_argument_exception ("media::collada::Effect::SetParam", "param", param);
    
  impl->shader_params [param] = value;
}
