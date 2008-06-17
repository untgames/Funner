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

Material::Material (Impl* in_impl)
  : impl (in_impl, false)
    {}
    
Material::Material (const Material& mtl)
  : impl (mtl.impl)
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
    Создание копии
*/

Material Material::Clone () const
{
  return Material (new Impl (*impl));
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
    throw xtl::make_null_argument_exception ("media::collada::Material::SetId", "id");
    
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
    throw xtl::make_null_argument_exception ("media::collada::Material::SetEffect", "effect_id");

  impl->effect = effect_id;
}
