#include "shared.h"

using namespace physics;

/*
   Геометрическое тело
*/

struct Shape::Impl : public xtl::reference_counter
{
  ShapePtr  shape;
  ShapeList subshapes;

  Impl (ShapePtr in_shape)
    : shape (in_shape)
    {}

  Impl (ShapePtr in_shape, const ShapeList& in_subshapes)
    : shape (in_shape), subshapes (in_subshapes)
    {}
};

/*
   Конструктор / деструктор / копирование
*/

Shape::Shape (Impl* source_impl)
  : impl (source_impl)
  {}

Shape::Shape (const Shape& source)
  : impl (source.impl)
{
  addref (impl);
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
   Толщина полей
*/

void Shape::SetMargin (float value)
{
  impl->shape->SetMargin (value);
}

float Shape::Margin () const
{
  return impl->shape->Margin ();
}

/*
   Обмен
*/

void Shape::Swap (Shape& shape)
{
  stl::swap (impl, shape.impl);
}

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

/*
   Создание
*/

Shape ShapeImplProvider::CreateShape (ShapePtr shape)
{
  return Shape (new Shape::Impl (shape));
}

Shape ShapeImplProvider::CreateShape (ShapePtr shape, const ShapeList& shape_list)
{
  return Shape (new Shape::Impl (shape, shape_list));
}

Shape ShapeImplProvider::CreateShape (physics::low_level::IDriver* driver, const media::physics::Shape& shape)
{
  if (!driver)
    throw xtl::make_null_argument_exception ("physics::low_level::IDriver*, const media::physics::Shape&", "driver");

  Shape return_value (CreateShapeCore (driver, shape));

  return_value.SetMargin (shape.Margin ());

  return return_value;
}

Shape ShapeImplProvider::CreateShape (physics::low_level::IDriver* driver, const ShapeList& shape_list)
{
  if (!driver)
    throw xtl::make_null_argument_exception ("physics::ShapeImplProvider::CreateShape (physics::low_level::IDriver*, const ShapeList&)", "driver");

  stl::vector<physics::low_level::IShape*>   shapes;
  stl::vector<physics::low_level::Transform> transforms;

  for (size_t i = 0, count = shape_list.Size (); i < count; i++)
  {
    const physics::Transform&     transform           = shape_list.Transform (i);
    physics::low_level::Transform low_level_transform = { transform.position, transform.orientation };

    shapes [i]     = ShapeImplProvider::LowLevelShape (shape_list [i]);
    transforms [i] = low_level_transform;
  }

  return ShapeImplProvider::CreateShape (ShapePtr (driver->CreateCompoundShape (shape_list.Size (), shapes.begin (), transforms.begin ()), false), shape_list);
}

Shape ShapeImplProvider::CreateShapeCore (physics::low_level::IDriver* driver, const media::physics::Shape& shape)
{
  static const char* METHOD_NAME = "physics::ShapeImplProvider::CreateShapeCore";

  switch (shape.Type ())
  {
    case media::physics::ShapeType_Box:
      return ShapeImplProvider::CreateShape (ShapePtr (driver->CreateBoxShape (shape.Data<media::physics::shapes::Box> ().half_dimensions), false));
    case media::physics::ShapeType_Sphere:
      return ShapeImplProvider::CreateShape (ShapePtr (driver->CreateSphereShape (shape.Data<media::physics::shapes::Sphere> ().radius), false));
    case media::physics::ShapeType_Capsule:
    {
      media::physics::shapes::Capsule shape_data = shape.Data<media::physics::shapes::Capsule> ();

      return ShapeImplProvider::CreateShape (ShapePtr (driver->CreateCapsuleShape (shape_data.radius, shape_data.height), false));
    }
    case media::physics::ShapeType_Plane:
    {
      media::physics::shapes::Plane shape_data = shape.Data<media::physics::shapes::Plane> ();

      return ShapeImplProvider::CreateShape (ShapePtr (driver->CreatePlaneShape (shape_data.normal, shape_data.d), false));
    }
    case media::physics::ShapeType_TriangleMesh:
    {
      media::physics::shapes::TriangleMesh shape_data = shape.Data<media::physics::shapes::TriangleMesh> ();

      return ShapeImplProvider::CreateShape (ShapePtr (driver->CreateTriangleMeshShape (shape_data.VerticesCount (), shape_data.Vertices (),
                                                                                        shape_data.TrianglesCount (), shape_data.Triangles () [0]), false));
    }
    case media::physics::ShapeType_Compound:
    {
      media::physics::shapes::Compound shape_data = shape.Data<media::physics::shapes::Compound> ();

      ShapeList shape_list;

      for (size_t i = 0, count = shape_data.Size (); i < count; i++)
      {
        const media::physics::shapes::Compound::ShapeTransform& transform = shape_data.Transform (i);

        shape_list.Add (CreateShapeCore (driver, shape_data.Shape (i)), transform.position, transform.orientation);
      }

      return CreateShape (driver, shape_list);
    }
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Unsupported media shape type %d", shape.Type ());
  }
}

/*
   Получение низкоуровневого тела
*/

physics::low_level::IShape* ShapeImplProvider::LowLevelShape (const Shape& shape)
{
  return shape.impl->shape.get ();
}
