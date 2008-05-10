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
    Описание реализации CommonMaterial
*/

struct CommonMaterial::Impl
{
  CommonMaterialShaderType shader_type;                      //тип шейдера
  vec3f                    colors [CommonMaterialColor_Num]; //цвета
  float                    shininess;                        //"металличность"
  float                    transparency;                     //прозрачность
  BlendFunction            blend_function;                   //функция смешивания
  CompareMode              alpha_test_mode;                  //режим сравнения аргументов при альфа-отсечении
  float                    alpha_test_reference;             //константа альфа-отсечения
  MapImpl                  maps [CommonMaterialMap_Num];     //текстурные карты

  Impl ();
};

/*
    Impl::Impl
*/

CommonMaterial::Impl::Impl ()
  : blend_function (make_blend_solid ())
{
  shader_type          = CommonMaterialShaderType_Default;
  shininess            = 0.0f;
  transparency         = 0.0f;
  alpha_test_mode      = CompareMode_AlwaysPass;
  alpha_test_reference = 0.0f;
}

/*
    Конструкторы / деструктор
*/

CommonMaterial::CommonMaterial ()
  : impl (new Impl)
  {}

CommonMaterial::CommonMaterial (const CommonMaterial& Material)
  : Material (Material),
    impl (new Impl (*Material.impl))
  {}

CommonMaterial::~CommonMaterial ()
{
}

/*
    Создание
*/

CommonMaterial::Pointer CommonMaterial::Create ()
{
  return Pointer (new CommonMaterial, false);
}

/*
    Реализация копирования
*/

Material* CommonMaterial::CloneCore () const
{
  return new CommonMaterial (*this);
}

/*
    Тип шейдера
*/

CommonMaterialShaderType CommonMaterial::ShaderType () const
{
  return impl->shader_type;
}

void CommonMaterial::SetShaderType (CommonMaterialShaderType type)
{
  if (type < 0 || type >= CommonMaterialShaderType_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetShaderType", "type", type);
    
  impl->shader_type = type;
}

/*
    Цвета
*/

const vec3f& CommonMaterial::Color (CommonMaterialColor color_id) const
{
  if (color_id < 0 || color_id >= CommonMaterialColor_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::Color", "color_id", color_id);
    
  return impl->colors [color_id];
}

void CommonMaterial::SetColor (CommonMaterialColor color_id, const vec3f& color)
{
  if (color_id < 0 || color_id >= CommonMaterialColor_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetColor", "color_id", color_id);
    
  impl->colors [color_id] = clamp (color, vec3f (0.0f), vec3f (1.0f));
}

void CommonMaterial::SetColor (CommonMaterialColor color_id, float red, float green, float blue)
{
  SetColor (color_id, vec3f (red, green, blue));
}

/*
    "Металличность"
*/

float CommonMaterial::Shininess () const
{
  return impl->shininess;
}

void CommonMaterial::SetShininess (float value)
{
  impl->shininess = clamp (value, 0.0f, 128.0f);
}

/*
    Прозрачность
*/

float CommonMaterial::Transparency () const
{
  return impl->transparency;
}

void CommonMaterial::SetTransparency (float value)
{
  impl->transparency = clamp (value, 0.0f, 1.0f);
}

/*
    Настройка режима смешивания цветов
*/

const BlendFunction& CommonMaterial::Blend () const
{
  return impl->blend_function;
}

void CommonMaterial::SetBlend (const BlendFunction& blend)
{
  if (blend.equation < 0 || blend.equation >= BlendEquation_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetBlend", "blend.equation", blend.equation);

  if (blend.argument [0] < 0 || blend.argument [0] >= BlendArgument_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetBlend", "blend.argument[0]", blend.argument [0]);

  if (blend.argument [1] < 0 || blend.argument [1] >= BlendArgument_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetBlend", "blend.argument[1]", blend.argument [1]);

  impl->blend_function = blend;
}

void CommonMaterial::SetBlend (BlendEquation equation, BlendArgument src_argument, BlendArgument dst_argument)
{
  SetBlend (BlendFunction (equation, src_argument, dst_argument));
}

/*
    Настройка альфа-отсечения
*/

CompareMode CommonMaterial::AlphaTestMode () const
{
  return impl->alpha_test_mode;
}

float CommonMaterial::AlphaTestReference () const
{
  return impl->alpha_test_reference;
}

void CommonMaterial::SetAlphaTestMode (CompareMode mode)
{
  if (mode < 0 || mode >= CompareMode_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetAlphaTestMode", "mode", mode);
    
  impl->alpha_test_mode = mode;
}

void CommonMaterial::SetAlphaTestReference (float alpha_reference)
{
  impl->alpha_test_reference = clamp (alpha_reference, 0.0f, 1.0f);
}

void CommonMaterial::SetAlphaTest (CompareMode mode, float alpha_reference)
{
  SetAlphaTestMode      (mode);
  SetAlphaTestReference (alpha_reference);
}

/*
    Текстурные карты
*/

const Texmap& CommonMaterial::Map (CommonMaterialMap map) const
{
  if (map < 0 || map >= CommonMaterialMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::Map", "map", map);
    
  return impl->maps [map].texmap;
}

Texmap& CommonMaterial::Map (CommonMaterialMap map)
{
  return const_cast<Texmap&> (const_cast<const CommonMaterial&> (*this).Map (map));
}

/*
    Веса текстурных карт
*/

float CommonMaterial::MapWeight (CommonMaterialMap map) const
{
  if (map < 0 || map >= CommonMaterialMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::MapWeight", "map", map);
    
  return impl->maps [map].weight;
}

void CommonMaterial::SetMapWeight (CommonMaterialMap map, float weight)
{
  if (map < 0 || map >= CommonMaterialMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetMapWeight", "map", map);

  impl->maps [map].weight = clamp (weight, 0.0f, 1.0f);
}

/*
    Разрешение / запрещение текстурных карт
*/

media::rfx::MapState CommonMaterial::MapState (CommonMaterialMap map) const
{
  if (map < 0 || map >= CommonMaterialMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::MapState", "map", map);
    
  return impl->maps [map].state;
}

void CommonMaterial::SetMapState (CommonMaterialMap map, media::rfx::MapState state)
{
  if (map < 0 || map >= CommonMaterialMap_Num)
    RaiseInvalidArgument ("media::rfx::CommonMaterial::SetMapState", "map", map);
    
  switch (state)
  {
    case MapState_Enabled:
    case MapState_Disabled:
      break;
    default:
      RaiseInvalidArgument ("media::rfx::CommonMaterial::SetMapState", "state", state);
      break;
  }

  impl->maps [map].state = state;
}

/*
    Динамическая диспетчеризация
*/

void CommonMaterial::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Material::AcceptCore (visitor);
}

/*
    Утилиты
*/

namespace media
{

namespace rfx
{ 

const char* get_name (BlendEquation id)
{
  switch (id)
  {
    case BlendEquation_Add:                return "add";
    case BlendEquation_Subtraction:        return "sub";
    case BlendEquation_ReverseSubtraction: return "rev_sub";
    case BlendEquation_Min:                return "min";
    case BlendEquation_Max:                return "max";
    default:                               RaiseInvalidArgument ("media::rfx::get_name(BlendEquation)", "id", id);
  }

  return "";
}

const char* get_name (BlendArgument id)
{
  switch (id)
  {
    case BlendArgument_Zero:                    return "zero";
    case BlendArgument_One:                     return "one";
    case BlendArgument_SourceColor:             return "src_color";
    case BlendArgument_SourceAlpha:             return "src_alpha";
    case BlendArgument_InverseSourceColor:      return "inv_src_color";
    case BlendArgument_InverseSourceAlpha:      return "inv_src_alpha";
    case BlendArgument_DestinationColor:        return "dst_color";
    case BlendArgument_DestinationAlpha:        return "dst_alpha";
    case BlendArgument_InverseDestinationColor: return "inv_dst_color";
    case BlendArgument_InverseDestinationAlpha: return "inv_dst_alpha";
    default:                                    RaiseInvalidArgument ("media::rfx::get_name(BlendArgument)", "id", id);
  }

  return "";
}

const char* get_name (CompareMode id)
{
  switch (id)
  {
    case CompareMode_AlwaysFail:   return "fail";
    case CompareMode_AlwaysPass:   return "pass";
    case CompareMode_Equal:        return "equal";
    case CompareMode_NotEqual:     return "not_equal";
    case CompareMode_Less:         return "less";
    case CompareMode_LessEqual:    return "less_equal";
    case CompareMode_Greater:      return "greater";
    case CompareMode_GreaterEqual: return "greater_equal";
    default:                       RaiseInvalidArgument ("media::rfx::get_name(CompareMode)", "id", id);
  }
  
  return "";
}

const char* get_name (CommonMaterialShaderType id)
{
  switch (id)
  {
    case CommonMaterialShaderType_Flat:    return "flat";
    case CommonMaterialShaderType_Gourand: return "gourand";
    case CommonMaterialShaderType_Phong:   return "phong";
    default:                         RaiseInvalidArgument ("media::rfx::get_name(CommonMaterialShaderType)", "id", id);
  }
  
  return "";
}

const char* get_name (CommonMaterialColor id)
{
  switch (id)
  {
    case CommonMaterialColor_Ambient:  return "ambient";
    case CommonMaterialColor_Diffuse:  return "diffuse";
    case CommonMaterialColor_Specular: return "specular";
    case CommonMaterialColor_Emission: return "emission";
    default:                           RaiseInvalidArgument ("media::rfx::get_name(CommonMaterialColor)", "id", id);
  }

  return "";
}

const char* get_name (CommonMaterialMap id)
{
  switch (id)
  {
    case CommonMaterialMap_Diffuse:     return "diffuse";
    case CommonMaterialMap_Ambient:     return "ambient";
    case CommonMaterialMap_Specular:    return "specular";
    case CommonMaterialMap_Transparent: return "transparency";
    case CommonMaterialMap_Emission:    return "emission";
    case CommonMaterialMap_Reflective:  return "reflective";
    case CommonMaterialMap_Bump:        return "bump";
    default:                            RaiseInvalidArgument ("media::rfx::get_name(CommonMaterialMap)", "id", id);
  }

  return "";
}

}

}
