#include "shared.h"

using namespace media::rfx;
using namespace math;
using namespace common;

/*
    Карта шейдера
*/

namespace
{

struct MapImpl
{
  Texmap   texmap; //текстурная карта
  float    weight; //вес карты
  MapState state;  //состояние карты
  
  MapImpl () : weight (1.0f), state (MapState_Disabled) {}
};

}

/*
    Описание реализации CommonShader
*/

struct CommonShader::Impl
{
  CommonShaderType shader_type;                    //тип шейдера
  vec3f            colors [CommonShaderColor_Num]; //цвета
  float            shininess;                      //"металличность"
  float            transparency;                   //прозрачность
  BlendFunction    blend_function;                 //функция смешивания
  CompareMode      alpha_test_mode;                //режим сравнения аргументов при альфа-отсечении 
  float            alpha_test_reference;           //константа альфа-отсечения
  MapImpl          maps [CommonShaderMap_Num];     //текстурные карты
  
  Impl ();
};

/*
    Impl::Impl
*/

CommonShader::Impl::Impl ()
  : blend_function (make_blend_solid ())
{
  shader_type          = CommonShaderType_Default;
  shininess            = 0.0f;
  transparency         = 0.0f;
  alpha_test_mode      = CompareMode_AlwaysPass;
  alpha_test_reference = 0.0f;
}

/*
    Конструкторы / деструктор
*/

CommonShader::CommonShader ()
  : impl (new Impl)
  {}

CommonShader::CommonShader (const CommonShader& shader)
  : Shader (shader),
    impl (new Impl (*shader.impl))
  {}

CommonShader::~CommonShader ()
{
  delete impl;
}

/*
    Создание
*/

CommonShader::Pointer CommonShader::Create ()
{
  return Pointer (new CommonShader, false);
}

/*
    Реализация копирования
*/

Shader* CommonShader::CloneCore () const
{
  return new CommonShader (*this);
}

/*
    Тип шейдера
*/

CommonShaderType CommonShader::Type () const
{
  return impl->shader_type;
}

void CommonShader::SetType (CommonShaderType type)
{
  if (type < 0 || type >= CommonShaderType_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetShaderType", "type", type);
    
  impl->shader_type = type;
}

/*
    Цвета
*/

const vec3f& CommonShader::Color (CommonShaderColor color_id) const
{
  if (color_id < 0 || color_id >= CommonShaderColor_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::Color", "color_id", color_id);
    
  return impl->colors [color_id];
}

void CommonShader::SetColor (CommonShaderColor color_id, const vec3f& color)
{
  if (color_id < 0 || color_id >= CommonShaderColor_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetColor", "color_id", color_id);
    
  impl->colors [color_id] = clamp (color, vec3f (0.0f), vec3f (1.0f));
}

void CommonShader::SetColor (CommonShaderColor color_id, float red, float green, float blue)
{
  SetColor (color_id, vec3f (red, green, blue));
}

/*
    "Металличность"
*/

float CommonShader::Shininess () const
{
  return impl->shininess;
}

void CommonShader::SetShininess (float value)
{
  impl->shininess = clamp (value, 0.0f, 128.0f);
}

/*
    Прозрачность
*/

float CommonShader::Transparency () const
{
  return impl->transparency;
}

void CommonShader::SetTransparency (float value)
{
  impl->transparency = clamp (value, 0.0f, 1.0f);
}

/*
    Настройка режима смешивания цветов
*/

const BlendFunction& CommonShader::Blend () const
{
  return impl->blend_function;
}

void CommonShader::SetBlend (const BlendFunction& blend)
{
  if (blend.equation < 0 || blend.equation >= BlendEquation_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetBlend", "blend.equation", blend.equation);

  if (blend.argument [0] < 0 || blend.argument [0] >= BlendArgument_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetBlend", "blend.argument[0]", blend.argument [0]);

  if (blend.argument [1] < 0 || blend.argument [1] >= BlendArgument_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetBlend", "blend.argument[1]", blend.argument [1]);

  impl->blend_function = blend;
}

void CommonShader::SetBlend (BlendEquation equation, BlendArgument src_argument, BlendArgument dst_argument)
{
  SetBlend (BlendFunction (equation, src_argument, dst_argument));
}

/*
    Настройка альфа-отсечения
*/

CompareMode CommonShader::AlphaTestMode () const
{
  return impl->alpha_test_mode;
}

float CommonShader::AlphaTestReference () const
{
  return impl->alpha_test_reference;
}

void CommonShader::SetAlphaTestMode (CompareMode mode)
{
  if (mode < 0 || mode >= CompareMode_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetAlphaTestMode", "mode", mode);
    
  impl->alpha_test_mode = mode;
}

void CommonShader::SetAlphaTestReference (float alpha_reference)
{
  impl->alpha_test_reference = clamp (alpha_reference, 0.0f, 1.0f);
}

void CommonShader::SetAlphaTest (CompareMode mode, float alpha_reference)
{
  SetAlphaTestMode      (mode);
  SetAlphaTestReference (alpha_reference);
}

/*
    Текстурные карты
*/

const Texmap& CommonShader::Map (CommonShaderMap map) const
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::Map", "map", map);
    
  return impl->maps [map].texmap;
}

Texmap& CommonShader::Map (CommonShaderMap map)
{
  return const_cast<Texmap&> (const_cast<const CommonShader&> (*this).Map (map));
}

/*
    Веса текстурных карт
*/

float CommonShader::MapWeight (CommonShaderMap map) const
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::MapWeight", "map", map);
    
  return impl->maps [map].weight;
}

void CommonShader::SetMapWeight (CommonShaderMap map, float weight)
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetMapWeight", "map", map);

  impl->maps [map].weight = clamp (weight, 0.0f, 1.0f);
}

/*
    Разрешение / запрещение текстурных карт
*/

media::rfx::MapState CommonShader::MapState (CommonShaderMap map) const
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::MapState", "map", map);
    
  return impl->maps [map].state;
}

void CommonShader::SetMapState (CommonShaderMap map, media::rfx::MapState state)
{
  if (map < 0 || map >= CommonShaderMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonShader::SetMapState", "map", map);
    
  switch (state)
  {
    case MapState_Enabled:
    case MapState_Disabled:
      break;
    default:
      RaiseInvalidArgument ("media::rfx::CommonShader::SetMapState", "state", state);
      break;
  }

  impl->maps [map].state = state;
}

/*
    Динамическая диспетчеризация
*/

void CommonShader::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Shader::AcceptCore (visitor);
}
