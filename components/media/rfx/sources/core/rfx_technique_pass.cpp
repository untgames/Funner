#include "shared.h"

using namespace media::rfx;

/*
    Реализация прохода техники рендеринга
*/

struct TechniquePass::Impl: public xtl::reference_counter
{
  common::PropertyMap properties; //свойства прохода
  
  Impl ()
  {
  }
  
  Impl (const Impl& impl)
    : properties (impl.properties.Clone ())
  {
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

TechniquePass::TechniquePass ()
  : impl (new Impl)
{
}

TechniquePass::TechniquePass (Impl* in_impl)
  : impl (in_impl)
{
}

TechniquePass::TechniquePass (const TechniquePass& pass)
  : impl (pass.impl)
{
  addref (impl);
}

TechniquePass::~TechniquePass ()
{
  release (impl);
}

TechniquePass& TechniquePass::operator = (const TechniquePass& pass)
{
  TechniquePass (pass).Swap (*this);
  
  return *this;
}

/*
    Копирование
*/

TechniquePass TechniquePass::Clone () const
{
  return TechniquePass (new Impl (*impl));
}

/*
    Идентификатор
*/

size_t TechniquePass::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Свойства прохода - параметры рендеринга
*/

common::PropertyMap TechniquePass::Properties () const
{
  return impl->properties;
}

/*
    Обмен
*/

void TechniquePass::Swap (TechniquePass& pass)
{
  stl::swap (impl, pass.impl);
}

namespace media
{

namespace rfx
{

void swap (TechniquePass& pass1, TechniquePass& pass2)
{
  pass1.Swap (pass2);
}

}

}
