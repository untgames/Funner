#include <media/collada/material.h>
#include <common/exception.h>

using namespace medialib::collada;
using namespace common;

/*
    Реализация эффекта
*/

namespace
{

class TextureImpl: public Texture
{
  public:
    TextureImpl () {}
    ~TextureImpl () {}
};

}

struct Map
{
  TextureImpl* texture; //текстура
  math::vec4f  color;   //цвет карты  
  
  Map () : texture (0) {}
};

struct Effect::Impl
{
  medialib::collada::ShaderType  shader_type;                     //тип шейдера
  Map                            maps [TextureMap_Num];           //карты
  float                          shader_params [EffectParam_Num]; //параметры шейдинга
  
  Impl () : shader_type (ShaderType_Default)
  {
    for (size_t i=0; i<EffectParam_Num; i++)
      shader_params [i] = 0.0f;
  }
  
  ~Impl ()
  {
    for (size_t i=0; i<TextureMap_Num; i++)
      delete maps [i].texture;
  }
};

/*
    Конструктор / деструктор
*/

Effect::Effect (ModelImpl* owner, const char* id)
  : Entity (owner, id),
    impl (new Impl)
  {}

Effect::~Effect ()
{
  delete impl;
}

/*
    Тип шейдера
*/

ShaderType Effect::ShaderType () const
{
  return impl->shader_type;
}

void Effect::SetShaderType (medialib::collada::ShaderType type)
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
      RaiseInvalidArgument ("medialib::collada::Effect::SetShaderType", "type", type);
      break;
  }
}
  
/*
    Работа с текстурными картами
*/

bool Effect::HasTexture (TextureMap map) const
{
  if (map < 0 || map >= TextureMap_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::HasTexture", "map", map);
    
  return impl->maps [map].texture != 0;
}

const Texture& Effect::Texture (TextureMap map) const
{
  if (map < 0 || map >= TextureMap_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::Texture", "map", map);
    
  medialib::collada::Texture* texture = impl->maps [map].texture;
  
  if (!texture)
    RaiseInvalidArgument ("medialib::collada::Effect::Texture", "map", map, "There is no texture attached to this map");

  return *texture;
}

Texture& Effect::Texture (TextureMap map)
{
  return const_cast<medialib::collada::Texture&> (const_cast<const Effect&> (*this).Texture (map));
}

Texture& Effect::CreateTexture (TextureMap map)
{
  if (map < 0 || map >= TextureMap_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::CreateTexture", "map", map);
    
  medialib::collada::Texture* texture = impl->maps [map].texture;
  
  if (!texture)
    texture = impl->maps [map].texture = new TextureImpl;

  return *texture;
}

void Effect::RemoveTexture (TextureMap map)
{
  if (map < 0 || map >= TextureMap_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::CreateTexture", "map", map);
    
  TextureImpl* texture = impl->maps [map].texture;
  
  if (!texture)
    return;

  delete texture;

  impl->maps [map].texture = 0;
}

/*
    Работа с цветом карт
*/

const math::vec4f& Effect::MapColor (TextureMap map) const
{
  if (map < 0 || map >= TextureMap_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::MapColor", "map", map);
    
  return impl->maps [map].color;
}

void Effect::SetMapColor (TextureMap map, const math::vec4f& color)
{
  if (map < 0 || map >= TextureMap_Num)
    RaiseInvalidArgument ("Effect::SetMapColor", "map", map);
    
  impl->maps [map].color = color;
}

/*
    Параметры шейдинга
*/

float Effect::Param (EffectParam param) const
{
  if (param < 0 || param >= EffectParam_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::Param", "param", param);
    
  return impl->shader_params [param];
}

void Effect::SetParam (EffectParam param, float value)
{
  if (param < 0 || param >= EffectParam_Num)
    RaiseInvalidArgument ("medialib::collada::Effect::SetParam", "param", param);
    
  impl->shader_params [param] = value;
}
