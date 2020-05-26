#include "shared.h"

using namespace common;
using namespace script;
using namespace xtl;

namespace
{

const char* COMMON_PROPERTY_MAP_LIBRARY = "Common.PropertyMap";

/*
   Регистрация библиотеки работы со свойствами узла
*/

template <class Ret> struct result_value
{
  Ret value;
  
  result_value () {}
};

template <class Ret> Ret get_property (PropertyMap& properties, const char* name)
{
  result_value<Ret> result;
  
  properties.GetProperty (name, result.value);
  
  return result.value;
}

template <class Ret> Ret get_property_element (PropertyMap& properties, const char* name, size_t element_index)
{
  xtl::uninitialized_storage<Ret> result;

  size_t elements_count = properties.PropertyElementsCount (name);

  if (element_index >= elements_count)
    throw xtl::make_range_exception ("engine::script_binds::get_property_element", "element_index", element_index, 0u, elements_count);

  result.resize (elements_count);

  properties.GetProperty (name, elements_count, result.data ());

  return result.data () [element_index];
}

template <class T> void set_property_element (PropertyMap& properties, const char* name, size_t element_index, const T& value)
{
  xtl::uninitialized_storage<T> values;

  size_t elements_count = properties.PropertyElementsCount (name);

  if (element_index >= elements_count)
    throw xtl::make_range_exception ("engine::script_binds::get_property_element", "element_index", element_index, 0u, elements_count);

  values.resize (elements_count);

  properties.GetProperty (name, elements_count, values.data ());

  values.data () [element_index] = value;

  properties.SetProperty (name, elements_count, values.data ());
}

PropertyMap create_property_map ()
{
  return PropertyMap ();
}

void set_vector_array (PropertyMap& properties, const char* name, const math::vec4f& vec1, const math::vec4f& vec2, const math::vec4f& vec3, const math::vec4f& vec4)
{
  math::vec4f values [4] = { vec1, vec2, vec3, vec4 };

  properties.SetProperty (name, 4, values);
}

void set_vector_array (PropertyMap& properties, const char* name, const math::vec4f& vec1, const math::vec4f& vec2, const math::vec4f& vec3)
{
  math::vec4f values [3] = { vec1, vec2, vec3 };

  properties.SetProperty (name, 3, values);
}

}

namespace engine
{

void bind_common_property_map (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_PROPERTY_MAP_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_property_map));
  
    //регистрация методов
    
  lib.Register ("get_Id",          make_invoker (&PropertyMap::Id));
  lib.Register ("get_Size",        make_invoker (&PropertyMap::Size));
  lib.Register ("get_Hash",        make_invoker (&PropertyMap::Hash));
  lib.Register ("get_LayoutHash",  make_invoker (&PropertyMap::LayoutHash));
  lib.Register ("SetPropertyName", make_invoker (implicit_cast<void (PropertyMap::*)(const char*, const char*)> (&PropertyMap::SetPropertyName)));
  lib.Register ("GetPropertyName", make_invoker (&PropertyMap::PropertyName));
  lib.Register ("RemoveProperty",  make_invoker (implicit_cast<void (PropertyMap::*)(const char*)> (&PropertyMap::RemoveProperty)));
  lib.Register ("Clear",           make_invoker (&PropertyMap::Clear));
  lib.Register ("Clone",           make_invoker (&PropertyMap::Clone));
  lib.Register ("GetPropertyType", make_invoker (
    make_invoker (implicit_cast<PropertyType (PropertyMap::*)(const char*) const> (&PropertyMap::PropertyType)),
    make_invoker (implicit_cast<PropertyType (PropertyMap::*)(size_t) const> (&PropertyMap::PropertyType))
  ));  
  lib.Register ("SetPropertyType", make_invoker (implicit_cast<void (PropertyMap::*)(const char*, PropertyType)> (&PropertyMap::SetPropertyType)));
  lib.Register ("GetPropertyElementsCount", make_invoker (
    make_invoker (implicit_cast<size_t (PropertyMap::*)(const char*) const> (&PropertyMap::PropertyElementsCount)),
    make_invoker (implicit_cast<size_t (PropertyMap::*)(size_t) const> (&PropertyMap::PropertyElementsCount))
  ));
  lib.Register ("SetPropertyElementsCount", make_invoker (implicit_cast<void (PropertyMap::*)(const char*, size_t)> (&PropertyMap::SetPropertyElementsCount)));
  lib.Register ("AddProperty", make_invoker (
    make_invoker (&PropertyMap::AddProperty),
    make_invoker<size_t (PropertyMap&, const char*, PropertyType)> (xtl::bind (&PropertyMap::AddProperty, _1, _2, _3, 1u))
  ));
  lib.Register ("IsPresent",       make_invoker (&PropertyMap::IsPresent));
  lib.Register ("IndexOf",         make_invoker (&PropertyMap::IndexOf));
  lib.Register ("GetString",       make_invoker (&get_property<stl::string>));
  lib.Register ("GetInteger",      make_invoker (&get_property<int>));
  lib.Register ("GetFloat",        make_invoker (&get_property<float>));
  lib.Register ("GetVector",       make_invoker (&get_property<math::vec4f>));
  lib.Register ("GetMatrix",       make_invoker (&get_property<math::mat4f>));
  lib.Register ("SetString",       make_invoker (implicit_cast<void (PropertyMap::*)(const char*, const char*)> (&PropertyMap::SetProperty)));
  lib.Register ("SetInteger",      make_invoker (implicit_cast<void (PropertyMap::*)(const char*, int)> (&PropertyMap::SetProperty)));
  lib.Register ("SetFloat",        make_invoker (implicit_cast<void (PropertyMap::*)(const char*, float)> (&PropertyMap::SetProperty)));
  lib.Register ("SetVector",       make_invoker (implicit_cast<void (PropertyMap::*)(const char*, const math::vec4f&)> (&PropertyMap::SetProperty)));
  lib.Register ("SetMatrix",       make_invoker (implicit_cast<void (PropertyMap::*)(const char*, const math::mat4f&)> (&PropertyMap::SetProperty)));
  lib.Register ("GetIntegerPropertyElement", make_invoker (&get_property_element<int>));
  lib.Register ("GetFloatPropertyElement",   make_invoker (&get_property_element<float>));
  lib.Register ("GetVectorPropertyElement",  make_invoker (&get_property_element<math::vec4f>));
  lib.Register ("GetMatrixPropertyElement",  make_invoker (&get_property_element<math::mat4f>));
  lib.Register ("SetIntegerPropertyElement", make_invoker (&set_property_element<int>));
  lib.Register ("SetFloatPropertyElement",   make_invoker (&set_property_element<float>));
  lib.Register ("SetVectorPropertyElement",  make_invoker (&set_property_element<math::vec4f>));
  lib.Register ("SetMatrixPropertyElement",  make_invoker (&set_property_element<math::mat4f>));

    //регистрация типов данных

  environment.RegisterType<PropertyMap> (COMMON_PROPERTY_MAP_LIBRARY);
}

}
