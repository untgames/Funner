#include "shared.h"

using namespace media::animation;

/*
    Описание реализации карты имен свойств анимации
*/

typedef stl::hash_map<stl::hash_key<const char*>, stl::string> NameMap;

struct PropertyNameMap::Impl: public xtl::reference_counter
{
  NameMap names; //карта имен
};

/*
    Конструкторы / деструкор / присваивание
*/

PropertyNameMap::PropertyNameMap ()
  : impl (new Impl)
{
}

PropertyNameMap::PropertyNameMap (Impl* in_impl)
  : impl (in_impl)
{
}

PropertyNameMap::PropertyNameMap (const PropertyNameMap& map)
  : impl (map.impl)
{
  addref (impl);
}

PropertyNameMap::~PropertyNameMap ()
{
  release (impl);
}

PropertyNameMap& PropertyNameMap::operator = (const PropertyNameMap& map)
{
  PropertyNameMap (map).Swap (*this);
  return *this;
}

/*
    Копирование
*/

PropertyNameMap PropertyNameMap::Clone () const
{
  try
  {
    return PropertyNameMap (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::PropertyNameMap::Clone");
    throw;
  }
}

/*
    Установление соответствия
*/

void PropertyNameMap::Set (const char* channel_name, const char* property_name)
{
  static const char* METHOD_NAME = "media::animation::PropertyNameMap::Set";
  
  if (!channel_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "channel_name");
    
  if (!property_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "property_name");
  
  impl->names [channel_name] = property_name;
}

const char* PropertyNameMap::Get (const char* channel_name) const
{
  if (!channel_name)
    return "";
    
  NameMap::iterator iter = impl->names.find (channel_name);
  
  if (iter == impl->names.end ())
    return "";
    
  return iter->second.c_str ();
}

/*
    Сброс соответствий
*/

void PropertyNameMap::Reset (const char* channel_name)
{
  if (!channel_name)
    return;
    
  impl->names.erase (channel_name);
}

void PropertyNameMap::Clear ()
{
  impl->names.clear ();
}

/*
    Обмен
*/

void PropertyNameMap::Swap (PropertyNameMap& map)
{
  stl::swap (impl, map.impl);
}

namespace media
{

namespace animation
{

void swap (PropertyNameMap& map1, PropertyNameMap& map2)
{
  map1.Swap (map2);
}

}

}
