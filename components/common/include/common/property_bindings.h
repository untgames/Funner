#ifndef COMMON_PROPERTY_BINDINGS_HEADER
#define COMMON_PROPERTY_BINDINGS_HEADER

#include <xtl/functional_fwd>

#include <common/property_map.h>

namespace common
{

/*
    Связывание карты и свойств
*/

template <class Value>
void bind (PropertyMap& map, const char* property_name, xtl::function<void (Value)>& set_fn);

template <class Value>
void bind (PropertyMap& map, const char* property_name, xtl::function<Value ()>& get_fn);

template <class Value>
void bind (PropertyMap& map, const char* property_name, xtl::function<void (Value)>& set_fn, xtl::function<Value ()>& get_fn);

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap&, const char* property_name, const ObjectPtr& ptr, Value (ObjectType::*)());

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap&, const char* property_name, const ObjectPtr& ptr, Value (ObjectType::*)() const);

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap&, const char* property_name, ObjectPtr& ptr, void (ObjectType::*)(Value));

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap&, const char* property_name, ObjectPtr& ptr, void (ObjectType::*)(Value), Value (ObjectType::*)() const);

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap&, const char* property_name, ObjectPtr& ptr, void (ObjectType::*)(Value), Value (ObjectType::*)());

#include <common/detail/property_bindings.inl>

}

#endif
