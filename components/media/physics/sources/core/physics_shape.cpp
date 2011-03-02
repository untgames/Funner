#include "shared.h"

using namespace media::physics;
using namespace media::physics::shapes;

namespace
{

const float DEFAULT_MARGIN = 0.f;

const char* get_type_name (ShapeType type)
{
  switch (type)
  {
    case ShapeType_Box:          return "Box";
    case ShapeType_Sphere:       return "Sphere";
    case ShapeType_Capsule:      return "Capsule";
    case ShapeType_Plane:        return "Plane";
    case ShapeType_TriangleMesh: return "TriangleMesh";
    case ShapeType_Compound:     return "Compound";
    default:                     return "Unknown";
  }
}

}

/*
   Реализация геометрического тела
*/

struct Shape::Impl : public xtl::reference_counter
{
  typedef xtl::shared_ptr <Shape::IShapeData> ShapeDataPtr;

  stl::string  name;   //имя тела
  float        margin; //толщина полей
  ShapeDataPtr data;   //данные тела

  Impl ()
    : margin (DEFAULT_MARGIN)
    {}

  Impl (const Impl& source)
    : name   (source.name)
    , margin (source.margin)
  {
    if (source.data)
      switch (source.data->Type ())
      {
        case ShapeType_Box:
        {
          const Box& source_data = static_cast<Shape::ShapeData <Box>* > (source.data.get ())->data;

          data.reset (new Shape::ShapeData<Box> (source_data));

          break;
        }
        case ShapeType_Sphere:
        {
          const Sphere& source_data = static_cast<Shape::ShapeData <Sphere>* > (source.data.get ())->data;

          data.reset (new Shape::ShapeData<Sphere> (source_data));

          break;
        }
        case ShapeType_Capsule:
        {
          const Capsule& source_data = static_cast<Shape::ShapeData <Capsule>* > (source.data.get ())->data;

          data.reset (new Shape::ShapeData<Capsule> (source_data));

          break;
        }
        case ShapeType_Plane:
        {
          const Plane& source_data = static_cast<Shape::ShapeData <Plane>* > (source.data.get ())->data;

          data.reset (new Shape::ShapeData<Plane> (source_data));

          break;
        }
        case ShapeType_TriangleMesh:
        {
          const TriangleMesh& source_data = static_cast<Shape::ShapeData <TriangleMesh>* > (source.data.get ())->data;

          data.reset (new Shape::ShapeData<TriangleMesh> (source_data.Clone ()));

          break;
        }
        case ShapeType_Compound:
        {
          const Compound& source_data = static_cast<Shape::ShapeData <Compound>* > (source.data.get ())->data;

          data.reset (new Shape::ShapeData<Compound> (source_data.Clone ()));

          break;
        }
        default:
          throw xtl::format_operation_exception ("media::physics::Shpe::Impl::Impl (const Impl&)", "Unknown source data type");
      }
  }
};

/*
   Конструкторы / деструктор / присваивание
*/

Shape::Shape ()
  : impl (new Impl)
{
}

Shape::Shape (const Shape& source)
  : impl (source.impl)
{
  addref (impl);
}

Shape::Shape (Impl* in_impl)
  : impl (in_impl)
{
}

Shape::~Shape ()
{
  release (impl);
}

Shape& Shape::operator = (const Shape& source)
{
  Shape (source).Swap (*this);

  return *this;
}
    
/*
   Создание копии
*/

Shape Shape::Clone () const
{
  return Shape (new Impl (*impl));
}

/*
   Идентификатор
*/

size_t Shape::Id () const
{
  return (size_t)impl;
}

/*
   Имя тела
*/

const char* Shape::Name () const
{
  return impl->name.c_str ();
}

void Shape::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::Shape::Rename", "name");

  impl->name = name;
}
    
/*
   Толщина полей
*/

float Shape::Margin () const
{
  return impl->margin;
}

void Shape::SetMargin (float value)
{
  impl->margin = value;
}

/*
   Получение типа
*/

ShapeType Shape::Type () const
{
  if (!impl->data)
    throw xtl::format_operation_exception ("media::physics::Shape::Type", "Shape data not setted");

  return impl->data->Type ();
}

/*
   Установка/получение описания
*/

void Shape::SetDataCore (IShapeData* data)
{
  impl->data.reset (data);
}

Shape::IShapeData* Shape::DataCore ()
{
  return impl->data.get ();
}

/*
   Выброс исключения
*/

void Shape::RaiseNullArgument (const char* source, const char* argument_name)
{
  throw xtl::make_null_argument_exception (source, argument_name);
}

void Shape::RaiseWrongType (ShapeType source_type, ShapeType required_type)
{
  throw xtl::format_operation_exception ("media::physics::Shape::RaiseWronType", "Incompatible shape data type requested, source type is '%s', required type is '%s'",
                                         get_type_name (source_type), get_type_name (required_type));
}

/*
   Обмен
*/

void Shape::Swap (Shape& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   Обмен
*/

void swap (Shape& shape1, Shape& shape2)
{
  shape1.Swap (shape2);
}

}

}
