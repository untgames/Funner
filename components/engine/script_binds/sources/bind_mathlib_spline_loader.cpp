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

    return_value.add_key (atof (child->Get (time_attribute)), atof (child->Get (value_attribute)));
  }

  return return_value;
}

linear_spline2f load_linear_vec2f_spline (const StringNode& node, const char* key_node, const char* time_attribute, const char* value_attribute, const char* delimeters)
{
  static const char* METHOD_NAME = "script::binds::load_linear_vec2f_spline";

  if (!key_node)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key_node");

  if (!time_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "time_attribute");

  if (!value_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value_attribute");

  if (!delimeters)
    throw xtl::make_null_argument_exception (METHOD_NAME, "delimeters");

  linear_spline2f return_value;

  return_value.reserve (node.ChildrenCount ());

  for (size_t i = 0, count = node.ChildrenCount (); i < count; i++)
  {
    StringNode::Pointer child = node.Child (i);

    if (xtl::xstrcmp (child->Name (), key_node))
      continue;

    common::StringArray tokens = common::split (child->Get (value_attribute), delimeters);

    switch (tokens.Size ())
    {
      case 1:
        return_value.add_key (atof (child->Get (time_attribute)), vec2f (atof (tokens [0])));
        break;
      case 2:
        return_value.add_key (atof (child->Get (time_attribute)), vec2f (atof (tokens [0]), atof (tokens [1])));
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Key %u value %s has invalid tokens count", i, child->Get (value_attribute));
    }
  }

  return return_value;
}

linear_spline3f load_linear_vec3f_spline (const StringNode& node, const char* key_node, const char* time_attribute, const char* value_attribute, const char* delimeters)
{
  static const char* METHOD_NAME = "script::binds::load_linear_vec3f_spline";

  if (!key_node)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key_node");

  if (!time_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "time_attribute");

  if (!value_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value_attribute");

  if (!delimeters)
    throw xtl::make_null_argument_exception (METHOD_NAME, "delimeters");

  linear_spline3f return_value;

  return_value.reserve (node.ChildrenCount ());

  for (size_t i = 0, count = node.ChildrenCount (); i < count; i++)
  {
    StringNode::Pointer child = node.Child (i);

    if (xtl::xstrcmp (child->Name (), key_node))
      continue;

    common::StringArray tokens = common::split (child->Get (value_attribute), delimeters);

    switch (tokens.Size ())
    {
      case 1:
        return_value.add_key (atof (child->Get (time_attribute)), vec3f (atof (tokens [0])));
        break;
      case 3:
        return_value.add_key (atof (child->Get (time_attribute)), vec3f (atof (tokens [0]), atof (tokens [1]), atof (tokens [2])));
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Key %u value %s has invalid tokens count", i, child->Get (value_attribute));
    }
  }

  return return_value;
}

linear_spline4f load_linear_vec4f_spline (const StringNode& node, const char* key_node, const char* time_attribute, const char* value_attribute, const char* delimeters)
{
  static const char* METHOD_NAME = "script::binds::load_linear_vec4f_spline";

  if (!key_node)
    throw xtl::make_null_argument_exception (METHOD_NAME, "key_node");

  if (!time_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "time_attribute");

  if (!value_attribute)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value_attribute");

  if (!delimeters)
    throw xtl::make_null_argument_exception (METHOD_NAME, "delimeters");

  linear_spline4f return_value;

  return_value.reserve (node.ChildrenCount ());

  for (size_t i = 0, count = node.ChildrenCount (); i < count; i++)
  {
    StringNode::Pointer child = node.Child (i);

    if (xtl::xstrcmp (child->Name (), key_node))
      continue;

    common::StringArray tokens = common::split (child->Get (value_attribute), delimeters);

    switch (tokens.Size ())
    {
      case 1:
        return_value.add_key (atof (child->Get (time_attribute)), vec4f (atof (tokens [0])));
        break;
      case 4:
        return_value.add_key (atof (child->Get (time_attribute)), vec4f (atof (tokens [0]), atof (tokens [1]), atof (tokens [2]), atof (tokens [3])));
        break;
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
      make_invoker (&load_linear_vec2f_spline),
      make_invoker<linear_spline2f (const StringNode&, const char*, const char*, const char*)> (xtl::bind (&load_linear_vec2f_spline, _1, _2, _3, _4, " "))
  ));
  lib.Register ("LoadLinearVec3fSpline", make_invoker (
      make_invoker (&load_linear_vec3f_spline),
      make_invoker<linear_spline3f (const StringNode&, const char*, const char*, const char*)> (xtl::bind (&load_linear_vec3f_spline, _1, _2, _3, _4, " "))
  ));
  lib.Register ("LoadLinearVec4fSpline", make_invoker (
      make_invoker (&load_linear_vec4f_spline),
      make_invoker<linear_spline4f (const StringNode&, const char*, const char*, const char*)> (xtl::bind (&load_linear_vec4f_spline, _1, _2, _3, _4, " "))
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
