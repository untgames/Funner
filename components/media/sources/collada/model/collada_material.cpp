#include "shared.h"

using namespace media::collada;
using namespace media;
using namespace common;

/*
    Реализация библиотеки
*/

struct Material::Impl: public xtl::reference_counter
{
  stl::string effect; //эффект, связанный с материалом
  stl::string id;     //идентификатор материала  
};

/*
    Конструкторы / деструктор / присваивание
*/

Material::Material ()
  : impl (new Impl, false)
    {}
    
Material::Material (const Material& mtl, media::CloneMode mode)
  : impl (media::clone (mtl.impl, mode, "media::collada::Material::Material"))
  {}

Material::~Material ()
{
}

Material& Material::operator = (const Material& mtl)
{
  impl = mtl.impl;
  
  return *this;
}

/*
    Идентификатор материала
*/

const char* Material::Id () const
{
  return impl->id.c_str ();
}

void Material::SetId (const char* id)
{
  if (!id)
    RaiseNullArgument ("media::collada::Material::SetId", "id");
    
  impl->id = id;
}

/*
    Эффект связанный с материалом
*/

const char* Material::Effect () const
{
  return impl->effect.c_str ();
}

void Material::SetEffect (const char* effect_id)
{
  if (!effect_id)
    RaiseNullArgument ("media::collada::Material::SetEffect", "effect_id");

  impl->effect = effect_id;
}
