#include "shared.h"

using namespace medialib::rfx;
using namespace medialib;
using namespace stl;
using namespace common;

const size_t DEFAULT_MATERIAL_ARRAY_RESERVE = 32;

/*
    Описание реализации библиотеки материалов
*/

typedef SharedResourceHolder<Material> MaterialHolder;
typedef vector<MaterialHolder>         MaterialArray;

struct MaterialLibrary::Impl: public SharedResource
{
  string        name;      //имя модели
  MaterialArray materials; //массив материалов

  Impl ();
  Impl (const Impl&);
};

/*
    MaterialModel::Impl::Impl
*/

MaterialLibrary::Impl::Impl ()
{
  materials.reserve (DEFAULT_MATERIAL_ARRAY_RESERVE);
}

MaterialLibrary::Impl::Impl (const Impl& impl)
  : name (impl.name)
{
  materials.reserve (impl.materials.size ());
  
  for (MaterialArray::const_iterator i=impl.materials.begin (), end=impl.materials.end (); i!=end; ++i)
    materials.push_back (MaterialHolder (*i, ForceClone));
}

/*
    MaterialLibrary
*/

/*
    Конструкторы / деструктор
*/

MaterialLibrary::MaterialLibrary ()
  : impl (new Impl)
  {}

MaterialLibrary::MaterialLibrary (const char* file_name)
  : impl (new Impl)
{
  if (!file_name)
    RaiseNullArgument ("medialib::MaterialLibrary::MaterialLibrary", "file_name");
    
  try
  {
    MaterialSystemSingleton::Instance ().Load (file_name, *this);
    
    Rename (file_name);
  }
  catch (common::Exception& exception)
  {
    exception.Raise ("medialib::MaterialLibrary::MaterialLibrary");
  }
}

MaterialLibrary::MaterialLibrary (const MaterialLibrary& library)
  : impl (new Impl (*library.impl))
  {}

MaterialLibrary::~MaterialLibrary ()
{
  delete impl;
}

/*
    Присваивание
*/

MaterialLibrary& MaterialLibrary::operator = (const MaterialLibrary& library)
{
  MaterialLibrary (library).Swap (*this);
  return *this;
}

/*
    Имя модели
*/

const char* MaterialLibrary::Name () const
{
  return impl->name.c_str ();
}

void MaterialLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("medialib::rfx::MaterialLibrary::Rename", name);
    
  impl->name = name;
}

/*
    Количество мешей
*/

size_t MaterialLibrary::MaterialsCount () const
{
  return impl->materials.size ();
}

/*
    Получение мешей
*/

const Material& MaterialLibrary::Material (size_t index) const
{
  if (index >= impl->materials.size ())
    RaiseOutOfRange ("medialib::rfx::MaterialLibrary::Material", "index", index, impl->materials.size ());
    
  return impl->materials [index].Resource ();
}

Material& MaterialLibrary::Material (size_t index)
{
  return const_cast<medialib::rfx::Material&> (const_cast<const MaterialLibrary&> (*this).Material (index));
}

/*
    Присоединение мешей
*/

size_t MaterialLibrary::Attach (medialib::rfx::Material& Material, CloneMode mode)
{
  impl->materials.push_back (MaterialHolder (Material, mode));

  return impl->materials.size () - 1;
}

void MaterialLibrary::Detach (size_t index)
{
  if (index >= impl->materials.size ())
    return;
    
  impl->materials.erase (impl->materials.begin () + index);
}

void MaterialLibrary::DetachAll ()
{
  impl->materials.clear ();
}

/*
    Загрузка / сохранение
*/

void MaterialLibrary::Load (const char* file_name)
{
  MaterialLibrary (file_name).Swap (*this);
}

void MaterialLibrary::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("medialib::MaterialLibrary::Save", "file_name");
    
  try
  {
    MaterialSystemSingleton::Instance ().Save (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Raise ("medialib::MaterialLibrary::Save");
  }
}

/*
    Обмен
*/

void MaterialLibrary::Swap (MaterialLibrary& library)
{
  stl::swap (library.impl, impl);
}

namespace medialib
{

namespace rfx
{

void swap (MaterialLibrary& material1, MaterialLibrary& material2)
{
  material1.Swap (material2);
}

}

}
