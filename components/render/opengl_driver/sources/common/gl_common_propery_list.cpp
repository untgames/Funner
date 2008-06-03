#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

const size_t PROPERTY_ARRAY_RESERVE = 8; //количество резервируемых свойств

/*
    Описание реализации PropertyList
*/

namespace
{

struct Property
{
  Property (const char* in_key, const char* in_value) : key (in_key), value (in_value) {}
  
  stl::string key, value;
};

}

typedef stl::vector<Property*> PropertyArray;

struct PropertyList::Impl
{
  PropertyArray properties; //свойства
};

/*
    Конструктор / деструктор
*/

PropertyList::PropertyList ()
  : impl (new Impl)
{
  impl->properties.reserve (PROPERTY_ARRAY_RESERVE);
}

PropertyList::~PropertyList ()
{
  Clear ();
}

/*
    Количество свойств
*/

size_t PropertyList::GetSize ()
{
  return impl->properties.size ();
}

/*
    Получение ключа и его значения
*/

const char* PropertyList::GetKey (size_t index)
{
  if (index >= impl->properties.size ())
    raise_out_of_range ("render::low_level::opengl::PropertyList::GetKey", "index", index, impl->properties.size ());
    
  return impl->properties [index]->key.c_str ();
}

const char* PropertyList::GetValue (size_t index)
{
  if (index >= impl->properties.size ())
    raise_out_of_range ("render::low_level::opengl::PropertyList::GetValue", "index", index, impl->properties.size ());
    
  return impl->properties [index]->value.c_str ();
}

/*
    Добавление / удаление свойств
*/

size_t PropertyList::AddProperty (const char* key, const char* value)
{
  if (!key)
    raise_null_argument ("render::low_level::opengl::PropertyList::AddProperty", "key");

  if (!value)
    raise_null_argument ("render::low_level::opengl::PropertyList::AddProperty", "value");

  Property* property = new Property (key, value);

  try
  {
    impl->properties.push_back (property);
    
    return impl->properties.size () - 1;
  }
  catch (...)
  {
    delete property;
    throw;
  }
}

void PropertyList::RemovePropery (size_t index)
{
  if (index >= impl->properties.size ())
    return;
    
  Property* property = impl->properties [index];
  
  impl->properties.erase (impl->properties.begin () + index);
  
  delete property;
}

/*
    Удаление всех свойств
*/

void PropertyList::Clear ()
{
  for (PropertyArray::iterator i=impl->properties.begin (), end=impl->properties.end (); i!=end; ++i)
    delete *i;

  impl->properties.clear ();
}
