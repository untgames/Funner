#include "shared.h"

using namespace physics;

/*
   Геометрическое тело
*/

struct ShapeImpl::Impl : public xtl::reference_counter
{
  ShapePtr shape;

  Impl (ShapePtr in_shape)
    : shape (in_shape)
    {}
};

/*
   Конструктор / деструктор / копирование
*/

ShapeImpl::ShapeImpl (ShapePtr shape)
  : impl (new Impl (shape))
  {}

ShapeImpl::ShapeImpl (const ShapeImpl& source)
  : impl (source.impl)
{
  addref (impl);
}

ShapeImpl::~ShapeImpl ()
{
  release (impl);
}

ShapeImpl& ShapeImpl::operator = (const ShapeImpl& source)
{
  ShapeImpl (source).Swap (*this);

  return *this;
}

/*
   Толщина полей
*/

void ShapeImpl::SetMargin (float value)
{
  impl->shape->SetMargin (value);
}

float ShapeImpl::Margin () const
{
  return impl->shape->Margin ();
}

/*
   Обмен
*/

void ShapeImpl::Swap (ShapeImpl& shape)
{
  stl::swap (impl, shape.impl);
}
