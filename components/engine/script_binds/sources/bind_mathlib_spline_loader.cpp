#include "shared.h"

using namespace engine;
using namespace script;
using namespace math;

namespace
{

//Константы (имена библиотек)

const char* BINDER_NAME                   = "MathSplineLoader";
const char* COMPONENT_NAME                = "script.binds.MathSplineLoader";
const char* MATHLIB_SPLINE_LOADER_LIBRARY = "Math.SplineLoader";

//Функции загрузки сплайнов
linear_splinef load_linear_float_spline (const StringNode& node, const char* key_node, const char* time_attribute, const char* value_attribute)
{
  static const char* METHOD_NAME = "script::binds::load_linear_float_spline";

  if (!key_node)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key_node");

  if (!time_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "time_attribute");

  if (!value_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value_attribute");

  linear_splinef return_value;

  return_value.reserve (node.ChildrenCount ());

  for (size_t i = 0, count = node.ChildrenCount (); i < count; i++)
  {
    StringNode::Pointer child = node.Child (i);

    if (xtl::xstrcmp (child->Name (), key_node))
      continue;

    float time;

    if (!xtl::io::read (child->Get (time_attribute), time))
      throw xtl::format_operation_exception (METHOD_NAME, "Key %u has invalid time value '%s'", i, child->Get (time_attribute));

    float value;

    if (!xtl::io::read (child->Get (value_attribute), value))
      throw xtl::format_operation_exception (METHOD_NAME, "Key %u has invalid value '%s'", i, child->Get (value_attribute));

    return_value.add_key (time, value);
  }

  return return_value;
}

template <class T, size_t Size>
basic_spline<spline_linear_key<vector<T, Size> > > load_linear_spline_vector_keys (const StringNode& node, const char* key_node, const char* time_attribute, const char* value_attribute, const char* delimeters)
{
  static const char* METHOD_NAME = "script::binds::load_linear_vector_spline";

  if (!key_node)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key_node");

  if (!time_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "time_attribute");

  if (!value_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value_attribute");

  if (!delimeters)
    throw xtl::make_null_argument_exception (METHOD_NAME, "delimeters");

  basic_spline<spline_linear_key<vector<T, Size> > > return_value;

  return_value.reserve (node.ChildrenCount ());

  for (size_t i = 0, count = node.ChildrenCount (); i < count; i++)
  {
    StringNode::Pointer child = node.Child (i);

    if (xtl::xstrcmp (child->Name (), key_node))
      continue;

    float time;

    if (!xtl::io::read (child->Get (time_attribute), time))
      throw xtl::format_operation_exception (METHOD_NAME, "Key %u has invalid time value '%s'", i, child->Get (time_attribute));

    common::StringArray tokens = common::split (child->Get (value_attribute), delimeters);

    switch (tokens.Size ())
    {
      case 1:
      {
        float value;

        if (!xtl::io::read (tokens [0], value))
          throw xtl::format_operation_exception (METHOD_NAME, "Key %u has invalid value '%s'", i, child->Get (value_attribute));

        return_value.add_key (time, value);
        break;
      }
      case Size:
      {
        math::vector <float, Size> v;

        for (unsigned char i = 0; i < Size; i++)
          if (!xtl::io::read (tokens [i], v [i]))
            throw xtl::format_operation_exception (METHOD_NAME, "Key %u has invalid value '%s'", i, child->Get (value_attribute));

        return_value.add_key (time, v);
        break;
      }
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Key %u value %s has invalid tokens count", i, child->Get (value_attribute));
    }
  }

  return return_value;
}

//Регистрация библиотеки загрузки сплайнов
void bind_math_spline_loader_library (script::Environment& environment)
{
  //создание библиотек
  
  InvokerRegistry lib  = environment.CreateLibrary (MATHLIB_SPLINE_LOADER_LIBRARY);

  //регистрация функций

  lib.Register ("LoadLinearFloatSpline", make_invoker (&load_linear_float_spline));
  lib.Register ("LoadLinearVec2fSpline", make_invoker (
      make_invoker (&load_linear_spline_vector_keys<float, 2>),
      make_invoker<linear_spline2f (const StringNode&, const char*, const char*, const char*)> (xtl::bind (&load_linear_spline_vector_keys<float, 2>, _1, _2, _3, _4, " "))
  ));
  lib.Register ("LoadLinearVec3fSpline", make_invoker (
      make_invoker (&load_linear_spline_vector_keys<float, 3>),
      make_invoker<linear_spline3f (const StringNode&, const char*, const char*, const char*)> (xtl::bind (&load_linear_spline_vector_keys<float, 3>, _1, _2, _3, _4, " "))
  ));
  lib.Register ("LoadLinearVec4fSpline", make_invoker (
      make_invoker (&load_linear_spline_vector_keys<float, 4>),
      make_invoker<linear_spline4f (const StringNode&, const char*, const char*, const char*)> (xtl::bind (&load_linear_spline_vector_keys<float, 4>, _1, _2, _3, _4, " "))
  ));
}

}

namespace components
{

namespace math_spline_loader_script_bind
{

//Компонент
class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_math_spline_loader_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> MathSplineLoaderScriptBind (COMPONENT_NAME);

}

}

}
