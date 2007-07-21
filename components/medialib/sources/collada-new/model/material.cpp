#include <media/collada/material.h>

using namespace medialib::collada;

/*
    Реализация библиотеки
*/

struct Material::Impl
{
  MaterialLibrary&           library; //библиотека
  medialib::collada::Effect& effect;  //эффект, связанный с материалом
  
  Impl (medialib::collada::Effect& in_effect, MaterialLibrary& in_library) : library (in_library), effect (in_effect) {}
};

/*
    Конструктор / деструктор
*/

Material::Material (medialib::collada::Effect& effect, MaterialLibrary& library, const char* id)
  : Entity (library.Owner (), id),
    impl (new Impl (effect, library))
    {}

Material::~Material ()
{
  delete impl;
}

/*
    Библиотека
*/

MaterialLibrary& Material::Library () const
{
  return impl->library;
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
