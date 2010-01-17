#include "shared.h"

using namespace media::rfx;

/*
    Описание реализации Material
*/

struct Material::Impl: public xtl::reference_counter
{
  stl::string         name;       //имя материала
  stl::string         effect;     //имя эффекта
  common::PropertyMap properties; //свойства материала
  
  Impl ()
  {
  }

  Impl (const Impl& impl)
    : name (impl.name)
    , effect (impl.effect)
    , properties (impl.properties.Clone ())
  {
  }
};

/*
    Конструкторы / деструктор
*/

Material::Material ()
  : impl (new Impl)
{
}

Material::Material (Impl* in_impl)
  : impl (in_impl)
{
}

Material::Material (const Material& material)
  : impl (material.impl)
{
  addref (impl);
}

Material::~Material ()
{
  release (impl);
}

Material& Material::operator = (const Material& material)
{
  Material (material).Swap (*this);
  
  return *this;
}

/*
    Копирование
*/

Material Material::Clone () const
{
  return Material (new Impl (*impl));
}

/*
    Идентификатор
*/
   
size_t Material::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Имя материала
*/

const char* Material::Name () const
{
  return impl->name.c_str ();
}

void Material::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Material::SetName", "name");
        
  impl->name = name;
}

/*
    Имя эффекта
*/

void Material::SetEffect (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Material::SetEffect", "name");
    
  impl->effect = name;
}

const char* Material::Effect () const
{
  return impl->effect.c_str ();
}

/*
    Свойства материала
*/

common::PropertyMap& Material::Properties ()
{
  return impl->properties;
}

const common::PropertyMap& Material::Properties () const
{
  return impl->properties;
}

/*
    Обмен
*/

void Material::Swap (Material& material)
{
  stl::swap (impl, material.impl);
}

namespace media
{

namespace rfx
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
