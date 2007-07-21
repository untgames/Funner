#include <media/collada/model.h>

#include <common/exception.h>
#include <stl/string>

#include "shared.h"
#include "library.h"

using namespace medialib::collada;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    Реализация модели
*/

namespace medialib
{

namespace collada
{

struct ModelImpl
{
  stl::string       name;      //имя модели
  Library<Effect>   effects;   //библиотека эффектов
  Library<Material> materials; //библиотека материалов
  
  ModelImpl () : effects (this), materials (this) {}
};

}

}

/*
    Утилиты
*/

namespace
{

void default_collada_log (const char*)
{
}

}

/*
    Конструкторы / деструктор
*/

Model::Model  ()
  : impl (new ModelImpl) {}

Model::Model (const char* file_name)
  : impl (new ModelImpl)
{
  ModelSystemSingleton::Instance ().Load (file_name, *this, &default_collada_log);
}

Model::Model (const char* file_name, const LogFunction& log)
  : impl (new ModelImpl)
{
  ModelSystemSingleton::Instance ().Load (file_name, *this, log);
}
  
Model::~Model ()
{
  delete impl;
}

/*
    Имя модели
*/

const char* Model::Name () const
{
  return impl->name.c_str ();
}

void Model::Rename (const char* new_name)
{
  if (!new_name)
    RaiseNullArgument ("medialib::collada::Model::Rename", "new_name");
    
  impl->name = new_name;
}

/*
    Загрузка / сохранение
*/

void Model::Load (const char* file_name)
{
  Model (file_name).Swap (*this);
}

void Model::Load (const char* file_name, const LogFunction& log)
{
  Model (file_name, log).Swap (*this);
}

void Model::Save (const char* file_name)
{
  ModelSystemSingleton::Instance ().Save (file_name, *this, &default_collada_log);
}

void Model::Save (const char* file_name, const LogFunction& log)
{
  ModelSystemSingleton::Instance ().Save (file_name, *this, log);
}

/*
    Библиотеки
*/

MaterialLibrary& Model::Materials ()
{
  return impl->materials;
}

const MaterialLibrary& Model::Materials () const
{
  return impl->materials;
}

EffectLibrary& Model::Effects ()
{
  return impl->effects;
}

const EffectLibrary& Model::Effects () const
{
  return impl->effects;
}

/*
    Обмен
*/

void Model::Swap (Model& model)
{
  stl::swap (model.impl, impl);
}

namespace medialib
{

namespace collada
{

void swap (Model& a, Model& b)
{
  a.Swap (b);
}

}

}
