#include "shared.h"

using namespace media::rfx;

/*
    Описание реализации параметра эффекта
*/

struct EffectParameter::Impl: public xtl::reference_counter
{
  stl::string         name;           //имя параметра
  EffectParameterType type;           //тип параметра
  size_t              elements_count; //количество элементов в параметре

  Impl ()
    : type (EffectParameterType_Int)
    , elements_count (1)
  {
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

EffectParameter::EffectParameter ()
  : impl (new Impl)
{
}

EffectParameter::EffectParameter (Impl* in_impl)
  : impl (in_impl)
{
}

EffectParameter::EffectParameter (const EffectParameter& param)
  : impl (param.impl)
{
  addref (impl);
}

EffectParameter::~EffectParameter ()
{
  release (impl);
}

EffectParameter& EffectParameter::operator = (const EffectParameter& param)
{
  EffectParameter (param).Swap (*this);
  
  return *this;
}

/*
    Копирование
*/

EffectParameter EffectParameter::Clone () const
{
  return EffectParameter (new Impl (*impl));
}

/*
    Идентификатор
*/

size_t EffectParameter::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Имя параметра
*/

const char* EffectParameter::Name () const
{
  return impl->name.c_str ();
}

void EffectParameter::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::EffectParameter::SetName", "name");
    
  impl->name = name;
}

/*
    Тип параметра
*/

EffectParameterType EffectParameter::Type () const
{
  return impl->type;
}

void EffectParameter::SetType (EffectParameterType type)
{
  switch (type)
  {
    case EffectParameterType_Int:
    case EffectParameterType_Float:
    case EffectParameterType_Int2:
    case EffectParameterType_Float2:
    case EffectParameterType_Int3:
    case EffectParameterType_Float3:
    case EffectParameterType_Int4:
    case EffectParameterType_Float4:
    case EffectParameterType_Float2x2:
    case EffectParameterType_Float3x3:
    case EffectParameterType_Float4x4:
      break;
    default:
      throw xtl::make_argument_exception ("media::rfx::EffectParameter::SetType", "type", type);
  }
  
  impl->type = type;
}

/*
    Количество элементов указанного типа в параметре
*/

size_t EffectParameter::ElementsCount () const
{
  return impl->elements_count;
}

void EffectParameter::SetElementsCount (size_t count)
{
  impl->elements_count = count;
}

/*
    Обмен
*/

void EffectParameter::Swap (EffectParameter& param)
{
  stl::swap (impl, param.impl);
}

namespace media
{

namespace rfx
{

/*
    Обмен
*/

void swap (EffectParameter& param1, EffectParameter& param2)
{
  param1.Swap (param2);
}

/*
    Имя типа параметр эффекта
*/

const char* get_name (EffectParameterType type)
{
  switch (type)
  {
    case EffectParameterType_Int:      return "int";
    case EffectParameterType_Float:    return "float";
    case EffectParameterType_Int2:     return "int2";
    case EffectParameterType_Float2:   return "float2";
    case EffectParameterType_Int3:     return "int3";
    case EffectParameterType_Float3:   return "float3";
    case EffectParameterType_Int4:     return "int4";
    case EffectParameterType_Float4:   return "float4";
    case EffectParameterType_Float2x2: return "float2x2";
    case EffectParameterType_Float3x3: return "float3x3";
    case EffectParameterType_Float4x4: return "float4x4";
    default:
      throw xtl::make_argument_exception ("media::rfx::get_name(EffectParameterType)", "type", type);
  }
}

}

}
