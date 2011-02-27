#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const size_t PROPERTY_ARRAY_RESERVE_SIZE = 8; //резервируемое количество свойств

}

/*
    Описание реализации расположения свойств
*/

typedef stl::vector<PropertyDesc> PropertyArray;

struct PropertyLayout::Impl: public xtl::reference_counter, public xtl::trackable
{
  PropertyArray properties;  //свойства
  StringArray   names;       //имена свойств
  size_t        buffer_size; //размер буфера
  size_t        hash;        //хэш
  bool          need_update; //необходимо обновить хэш
  
///Конструктор
  Impl ()
    : buffer_size (0)
    , hash (0)
    , need_update (true)
  {
    properties.reserve (PROPERTY_ARRAY_RESERVE_SIZE);
  }

///Конструктор копирования
  Impl (const Impl& impl)
    : properties (impl.properties)
    , names (impl.names)
    , buffer_size (impl.buffer_size)
    , hash (0)
    , need_update (true)
  {
    Update ();
  }
  
///Обновление
  void Update ()
  {
    buffer_size = 0;
    
    PropertyDesc* desc = &properties [0];

    for (size_t i=0; i<properties.size (); i++, desc++)
    {
      desc->name    = 0;
      desc->offset  = buffer_size;      
      buffer_size  += get_size (desc->type) * desc->elements_count;
    }
      
    hash = crc32 (&properties [0], properties.size () * sizeof (PropertyDesc), crc32 (names.Buffer (), names.BufferSize ()));

    desc = &properties [0];    

    for (size_t i=0; i<properties.size (); i++, desc++)
      desc->name = names [i];

    need_update = false;    
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

PropertyLayout::PropertyLayout ()
  : impl (new Impl)
{
}

PropertyLayout::PropertyLayout (Impl* in_impl)
  : impl (in_impl)
{
}

PropertyLayout::PropertyLayout (const PropertyLayout& layout)
  : impl (layout.impl)
{
  addref (impl);
}

PropertyLayout::~PropertyLayout ()
{
  release (impl);
}

PropertyLayout& PropertyLayout::operator = (const PropertyLayout& layout)
{
  PropertyLayout (layout).Swap (*this);
  
  return *this;
}

/*
    Идентификатор
*/

size_t PropertyLayout::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Получение объекта оповещения об удалении
*/

xtl::trackable& PropertyLayout::Trackable () const
{
  return *impl;
}

namespace common
{

xtl::trackable& get_trackable (const PropertyLayout& layout)
{
  return layout.Trackable ();
}

}

/*
    Получение числа ссылок на данный объект
*/

size_t PropertyLayout::UseCount () const
{
  return impl->use_count ();
}

/*
    Копирование
*/

PropertyLayout PropertyLayout::Clone () const
{
  try
  {
    return PropertyLayout (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyLayout::Clone");
    throw;
  }
}

/*
    Хэш структуры / размер буфера
*/

size_t PropertyLayout::Hash () const
{
  if (impl->need_update)
    impl->Update ();
    
  return impl->hash;
}

size_t PropertyLayout::BufferSize () const
{
  if (impl->need_update)
    impl->Update ();

  return impl->buffer_size;
}

/*
    Количество свойств
*/

size_t PropertyLayout::Size () const
{
  return impl->properties.size ();
}

/*
    Получение свойств
*/

const PropertyDesc* PropertyLayout::Properties () const
{
  if (impl->need_update)
    impl->Update ();

  return impl->properties.empty () ? 0 : &impl->properties [0];
}

const char* PropertyLayout::PropertyName (size_t property_index) const
{
  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception ("common::PropertyLayout::PropertyName", "property_index", property_index, impl->properties.size ());
    
  if (impl->need_update)
    impl->Update ();    

  return impl->properties [property_index].name;    
}

common::PropertyType PropertyLayout::PropertyType (size_t property_index) const
{
  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception ("common::PropertyLayout::PropertyType(size_t)", "property_index", property_index, impl->properties.size ());
    
  return impl->properties [property_index].type;
}

common::PropertyType PropertyLayout::PropertyType (const char* property_name) const
{
  static const char* METHOD_NAME = "common::PropertyLayout::PropertyType(const char*)";

  if (!property_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");

  int index = IndexOf (property_name);
  
  if (index < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "property_name", property_name, "There is no property with this name");
    
  return impl->properties [index].type;
}

size_t PropertyLayout::PropertyElementsCount (size_t property_index) const
{
  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception ("common::PropertyLayout::PropertyElementsCount(size_t)", "property_index", property_index, impl->properties.size ());
    
  return impl->properties [property_index].elements_count;
}

size_t PropertyLayout::PropertyElementsCount (const char* property_name) const
{
  static const char* METHOD_NAME = "common::PropertyLayout::PropertyElementsCount(const char*)";

  if (!property_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");

  int index = IndexOf (property_name);
  
  if (index < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "property_name", property_name, "There is no property with this name");

  return impl->properties [index].elements_count;
}

/*
    Добавление и удаление свойств
*/

size_t PropertyLayout::AddProperty (const char* name, common::PropertyType type, size_t elements_count)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!elements_count)
      throw xtl::make_null_argument_exception ("", "elements_count");
      
    switch (type)
    {
      case PropertyType_String:
      case PropertyType_Integer:
      case PropertyType_Float:
      case PropertyType_Vector:
      case PropertyType_Matrix:
        break;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }

    impl->need_update = true;

    size_t string_index = impl->names.Add (name);
    
    try
    {
      PropertyDesc desc;    

      desc.name           = impl->names [string_index];
      desc.name_hash      = strhash (desc.name);
      desc.type           = type;
      desc.elements_count = elements_count;

      impl->properties.push_back (desc);

      return impl->properties.size () - 1;
    }
    catch (...)
    {
      impl->names.Remove (string_index);
      throw;
    }
  }  
  catch (xtl::exception& e)
  {
    e.touch ("common::PropertyLayout::AddProperty");
    throw;
  }
}

void PropertyLayout::RemoveProperty (size_t property_index)
{
  if (property_index >= impl->properties.size ())
    return;
    
  impl->properties.erase (impl->properties.begin () + property_index);
  
  impl->names.Remove (property_index);
  
  impl->need_update = true;
}

void PropertyLayout::Clear ()
{
  impl->properties.clear ();
  impl->names.Clear ();

  impl->need_update = true;
}

/*
    Модификация свойств
*/

void PropertyLayout::SetPropertyName (const char* property_name, const char* new_name)
{
  static const char* METHOD_NAME = "common::PropertyLayout::SetPropertyName(const char*,const char*)";

  if (!property_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");

  int index = IndexOf (property_name);
  
  if (index < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "property_name", property_name, "There is no property with this name");
  
  SetPropertyName (static_cast<size_t> (index), new_name);
}

void PropertyLayout::SetPropertyType (const char* property_name, common::PropertyType type)
{
  static const char* METHOD_NAME = "common::PropertyLayout::SetPropertyType(const char*,PropertyType)";

  if (!property_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");

  int index = IndexOf (property_name);
  
  if (index < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "property_name", property_name, "There is no property with this name");
  
  SetPropertyType (static_cast<size_t> (index), type);
}

void PropertyLayout::SetPropertyElementsCount (const char* property_name, size_t elements_count)
{
  static const char* METHOD_NAME = "common::PropertyLayout::SetPropertyElementsCount(const char*,PropertyType)";

  if (!property_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");

  int index = IndexOf (property_name);
  
  if (index < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "property_name", property_name, "There is no property with this name");
  
  SetPropertyElementsCount (static_cast<size_t> (index), elements_count);
}

void PropertyLayout::SetPropertyName (size_t property_index, const char* name)
{
  static const char* METHOD_NAME = "common::PropertyLayout::SetPropertyType(size_t,PropertyType)";
  
  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  
  impl->names.Set (property_index, name);

  impl->properties [property_index].name_hash = strhash (name);

  impl->need_update = true;
}

void PropertyLayout::SetPropertyType (size_t property_index, common::PropertyType type)
{
  static const char* METHOD_NAME = "common::PropertyLayout::SetPropertyType(size_t,PropertyType)";
  
  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  switch (type)
  {
    case PropertyType_String:
    case PropertyType_Integer:
    case PropertyType_Float:
    case PropertyType_Vector:
    case PropertyType_Matrix:
      break;
    default:
      throw xtl::make_argument_exception ("", "type", type);
  }
  
  impl->properties [property_index].type = type;
  
  impl->need_update = true;
}

void PropertyLayout::SetPropertyElementsCount (size_t property_index, size_t elements_count)
{
  static const char* METHOD_NAME = "common::PropertyLayout::SetPropertyElementsCount(size_t,size_t)";

  if (property_index >= impl->properties.size ())
    throw xtl::make_range_exception (METHOD_NAME, "property_index", property_index, impl->properties.size ());

  if (!elements_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "elements_count");

  impl->properties [property_index].elements_count = elements_count;

  impl->need_update = true;
}

/*
    Поиск индекса свойства
*/

int PropertyLayout::IndexOf (const char* name) const
{
  if (!name)
    return -1;
    
  if (impl->need_update)
    impl->Update ();
    
  size_t hash  = common::strhash (name);
  int    index = 0;  

  for (PropertyArray::iterator iter=impl->properties.begin (), end=impl->properties.end (); iter!=end; ++iter, ++index)
    if (iter->name_hash == hash)
      return index;
      
  return -1;
}

bool PropertyLayout::IsPresent (const char* name) const
{
  return IndexOf (name) != -1;
}

/*
    Обмен
*/

void PropertyLayout::Swap (PropertyLayout& layout)
{
  stl::swap (impl, layout.impl);
}

namespace common
{

void swap (PropertyLayout& layout1, PropertyLayout& layout2)
{
  layout1.Swap (layout2);
}

}
