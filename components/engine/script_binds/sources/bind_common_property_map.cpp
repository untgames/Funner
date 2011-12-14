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

PropertyMap create_property_map ()
{
  return PropertyMap ();
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

    //регистрация типов данных

  environment.RegisterType<PropertyMap> (COMMON_PROPERTY_MAP_LIBRARY);
}

}
