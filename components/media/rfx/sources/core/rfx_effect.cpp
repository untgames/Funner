#include "shared.h"

using namespace media::rfx;

/*
    Константы
*/

namespace
{

const size_t TECHNIQUE_ARRAY_RESERVE = 16; //резервируемое число техник
const size_t PARAMETER_ARRAY_RESERVE = 8;  //резервируемое число параметров

}

/*
    Описание реализации эффекта
*/

typedef stl::vector<Technique>       TechniqueArray;
typedef stl::vector<EffectParameter> ParameterArray;

struct Effect::Impl: public xtl::reference_counter
{
  stl::string         name;       //имя эффекта
  TechniqueArray      techniques; //техники рендеринга
  ParameterArray      parameters; //параметры рендеринга
  common::PropertyMap properties; //свойства эффекта

  Impl ()
  {
    techniques.reserve (TECHNIQUE_ARRAY_RESERVE);
    parameters.reserve (PARAMETER_ARRAY_RESERVE);
  }
  
  Impl (const Impl& impl)
    : name (impl.name)
    , properties (impl.properties.Clone ())
  {
    techniques.reserve (impl.techniques.size ());
    parameters.reserve (impl.parameters.size ());
    
    for (TechniqueArray::const_iterator iter=impl.techniques.begin (), end=impl.techniques.end (); iter!=end; ++iter)
      techniques.push_back (iter->Clone ());
      
    for (ParameterArray::const_iterator iter=impl.parameters.begin (), end=impl.parameters.end (); iter!=end; ++iter)
      parameters.push_back (iter->Clone ());
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Effect::Effect ()
  : impl (new Impl)
{
}

Effect::Effect (Impl* in_impl)
  : impl (in_impl)
{
}

Effect::Effect (const Effect& effect)
  : impl (effect.impl)
{
  addref (impl);
}

Effect::~Effect ()
{
  release (impl);
}

Effect& Effect::operator = (const Effect& effect)
{
  Effect (effect).Swap (*this);
  
  return *this;
}

/*
    Копирование
*/

Effect Effect::Clone () const
{
  return Effect (new Impl (*impl));
}

/*
    Идентификатор
*/

size_t Effect::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Имя эффекта
*/

const char* Effect::Name () const
{
  return impl->name.c_str ();
}

void Effect::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Effect::SetName", "name");
    
  impl->name = name;
}

/*
    Количество техник
*/

size_t Effect::TechniquesCount () const
{
  return impl->techniques.size ();
}

/*
    Перебор техник
*/

const Technique& Effect::Technique (size_t index) const
{
  if (index >= impl->techniques.size ())
    throw xtl::make_range_exception ("media::rfx::Effect::Technique(size_t)", "index", index, impl->techniques.size ());
    
  return impl->techniques [index];
}

Technique& Effect::Technique (size_t index)
{
  return const_cast<media::rfx::Technique&> (const_cast<const Effect&> (*this).Technique (index));
}

const Technique& Effect::Technique (const char* name) const
{
  static const char* METHOD_NAME = "media::rfx::Effect::Technique(const char*)";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  int index = FindTechnique (name);
  
  if (index == -1)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "No technique with this name");
    
  return Technique (index);
}

Technique& Effect::Technique (const char* name)
{
  return const_cast<media::rfx::Technique&> (const_cast<const Effect&> (*this).Technique (name));
}

/*
    Поиск техник
*/

int Effect::FindTechnique (const char* name) const
{
  if (!name)
    return -1;
    
  for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
    if (!strcmp (iter->Name (), name))
      return iter - impl->techniques.begin ();
      
  return -1;
}

/*
    Добавление и удаление техник
*/

size_t Effect::AddTechnique (const media::rfx::Technique& technique)
{
  impl->techniques.push_back (technique);
  
  return impl->techniques.size () - 1;
}

void Effect::RemoveTechnique (size_t index)
{
  if (index >= impl->techniques.size ())
    return;
    
  impl->techniques.erase (impl->techniques.begin () + index);
}

void Effect::RemoveTechnique (const char* name)
{
  RemoveTechnique ((size_t)FindTechnique (name));
}

void Effect::RemoveAllTechniques ()
{
  impl->techniques.clear ();
}

/*
    Количество параметров
*/

size_t Effect::ParametersCount () const
{
  return impl->parameters.size ();
}

/*
    Перебор параметров
*/

const EffectParameter& Effect::Parameter (size_t index) const
{
  if (index >= impl->parameters.size ())
    throw xtl::make_range_exception ("media::rfx::Effect::Parameter(size_t)", "index", index, impl->parameters.size ());
    
  return impl->parameters [index];
}

EffectParameter& Effect::Parameter (size_t index)
{
  return const_cast<EffectParameter&> (const_cast<const Effect&> (*this).Parameter (index));
}

const EffectParameter& Effect::Parameter (const char* name) const
{
  static const char* METHOD_NAME = "media::rfx::Effect::Parameter(const char*)";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  int index = FindParameter (name);
  
  if (index == -1)
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "No parameter with this name");
    
  return Parameter ((size_t)index);
}

EffectParameter& Effect::Parameter (const char* name)
{
  return const_cast<EffectParameter&> (const_cast<const Effect&> (*this).Parameter (name));
}

/*
    Поиск параметров
*/

int Effect::FindParameter (const char* name) const
{
  if (!name)
    return -1;
    
  for (ParameterArray::iterator iter=impl->parameters.begin (), end=impl->parameters.end (); iter!=end; ++iter)
    if (!strcmp (iter->Name (), name))
      return iter - impl->parameters.begin ();
      
  return -1;
}

/*
    Добавление и удаление параметров
*/

size_t Effect::AddParameter (const EffectParameter& param)
{
  impl->parameters.push_back (param);
  
  return impl->parameters.size () - 1;
}

void Effect::RemoveParameter (size_t index)
{
  if (index >= impl->parameters.size ())
    return;
    
  impl->parameters.erase (impl->parameters.begin () + index);
}

void Effect::RemoveParameter (const char* name)
{
  RemoveParameter ((size_t)FindParameter (name));
}

void Effect::RemoveAllParameters ()
{
  impl->parameters.clear ();
}

/*
    Свойства техники - параметры рендеринга
*/

common::PropertyMap Effect::Properties () const
{
  return impl->properties;
}

/*
    Очистка
*/

void Effect::Clear ()
{
  RemoveAllTechniques ();
  RemoveAllParameters ();
  
  impl->name.clear ();
  impl->properties.Clear ();
}

/*
    Обмен
*/

void Effect::Swap (Effect& effect)
{
  stl::swap (effect.impl, impl);
}

namespace media
{

namespace rfx
{

void swap (Effect& effect1, Effect& effect2)
{
  effect1.Swap (effect2);
}

}

}
