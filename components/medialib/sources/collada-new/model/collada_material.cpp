#include <media/collada/material.h>

using namespace medialib::collada;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    Реализация библиотеки
*/

struct Material::Impl
{
  medialib::collada::Effect& effect;  //эффект, связанный с материалом
  
  Impl (Entity& entity, medialib::collada::Effect& in_effect) : effect (in_effect)
  {
    if (effect.Owner () != entity.Owner ())
      raise_incompatible ("medialib::collada::Material::Material", effect, entity);
  }
};

/*
    Конструктор / деструктор
*/

Material::Material (medialib::collada::Effect& effect, ModelImpl* owner, const char* id)
  : Entity (owner, id),
    impl (new Impl (*this, effect))
    {}

Material::~Material ()
{
  delete impl;
}

/*
    Эффект связанный с материалом
*/

const medialib::collada::Effect& Material::Effect () const
{
  return impl->effect;
}

medialib::collada::Effect& Material::Effect ()
{
  return impl->effect;
}
