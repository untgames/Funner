#include "shared.h"

using namespace media::geometry;
using namespace common;

/*
    Константы
*/

namespace
{

const size_t DEFAULT_ATTRIBUTES_RESERVE_SIZE = 4;

}

/*
    Описание реализации формата вершин
*/

typedef stl::vector<VertexAttribute> VertexAttributeArray;

struct VertexFormat::Impl
{
  VertexAttributeArray attributes;              //атрибуты вершины
  StringArray          names;                   //имена атрибутов
  size_t               attributes_hash;         //хэш от массива атрибутов
  bool                 need_hash_update;        //необходим пересчёт хэша атрибутов
  uint32_t             min_vertex_size;         //минимальный размер вершины
  bool                 need_vertex_size_update; //необходим пересчёт размера вершины

  Impl ()
    : need_hash_update (true) 
    , min_vertex_size ()
    , need_vertex_size_update ()
  {
  }

  size_t Hash ()
  {
    if (need_hash_update)
    {
      size_t names_hash = crc32 (names.Buffer (), names.BufferSize ());

      for (VertexAttributeArray::iterator iter=attributes.begin (), end=attributes.end (); iter!=end; ++iter)
        iter->name = 0;

      attributes_hash = crc32 (&attributes [0], sizeof (VertexAttribute) * attributes.size (), names_hash);

      const char** name = names.Data ();

      for (VertexAttributeArray::iterator iter=attributes.begin (), end=attributes.end (); iter!=end; ++iter, ++name)
        iter->name = *name;

      need_hash_update = false;
    }

    return attributes_hash;
  }

  void UpdateNames ()
  {
    const char** name = names.Data ();

    for (VertexAttributeArray::iterator iter=attributes.begin (), end=attributes.end (); iter!=end; ++iter, ++name)
      iter->name = *name;
  }  
};

/*
    Конструктор / деструктор
*/

VertexFormat::VertexFormat ()
  : impl (new Impl)
{
  ReserveAttributes (DEFAULT_ATTRIBUTES_RESERVE_SIZE);
}

VertexFormat::VertexFormat (const VertexFormat& vf)
  : impl (new Impl (*vf.impl))
  {}

VertexFormat::~VertexFormat ()
{
  delete impl;
}

/*
    Присваивание
*/

VertexFormat& VertexFormat::operator = (const VertexFormat& vf)
{
  VertexFormat (vf).Swap (*this);
  
  return *this;
}

/*
    Клонирование
*/

VertexFormat VertexFormat::Clone () const
{
  try
  {
    VertexFormat format;

    format.AddAttributes (*this);

    return format;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::geometry::VertexFormat::Clone");
    throw;
  }
}
  
/*
    Количество атрибутов
*/

uint32_t VertexFormat::AttributesCount () const
{
  return (uint32_t)impl->attributes.size ();
}

/*
    Резервирование количества атрибутов
*/

void VertexFormat::ReserveAttributes (uint32_t count, uint32_t name_buffer_size)
{
  impl->attributes.reserve (count);
  impl->names.Reserve (count);
  impl->names.ReserveBuffer (name_buffer_size);
}

uint32_t VertexFormat::ReservedAttributesCount () const
{
  return (uint32_t)impl->attributes.capacity ();
}

/*
    Получение массива атрибутов
*/

const VertexAttribute* VertexFormat::Attributes () const
{
  if (impl->attributes.empty ())
    return 0;

  return &impl->attributes [0];
}

/*
    Получение атрибута
*/

const VertexAttribute& VertexFormat::Attribute (uint32_t index) const
{
  if (index >= impl->attributes.size ())
    throw xtl::make_range_exception ("media::geometry::VertexFormat::Attribute", "index", index, impl->attributes.size ());

  return impl->attributes [index];
}

const char* VertexFormat::AttributeName (uint32_t index) const
{
  if (index >= impl->attributes.size ())
    throw xtl::make_range_exception ("media::geometry::VertexFormat::AttributeName", "index", index, impl->attributes.size ());

  return impl->names [index];
}

/*
    Поиск атрибута по семантике
*/

const VertexAttribute* VertexFormat::FindAttribute (VertexAttributeSemantic semantic, const VertexAttribute* after) const
{
  if (!after)
    after = &impl->attributes [0];

  VertexAttribute* end = &*impl->attributes.end ();

  if (after < &impl->attributes [0] || after >= end)
    return 0;

  for (const VertexAttribute* attribute=after; attribute != end; attribute++)
    if (attribute->semantic == semantic)   
      return attribute;
      
  return 0;
}

const VertexAttribute* VertexFormat::FindAttribute (const char* name) const
{
  if (!name)
    return 0;

  for (VertexAttribute *attribute=&impl->attributes [0], *end=&*impl->attributes.end (); attribute != end; attribute++)
    if (attribute->name && !strcmp (name, attribute->name))
      return attribute;

  return 0;
}

/*
    Добавление атрибутов
*/

uint32_t VertexFormat::AddAttribute (const char* name, VertexAttributeSemantic semantic, VertexAttributeType type, uint32_t offset)
{
  static const char* METHOD_NAME = "media::geometry::VertexFormat::AddAttribute";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!*name)
    name = "";

  if (semantic < 0 || semantic >= VertexAttributeSemantic_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "semantic", semantic);
    
  if (type < 0 || type >= VertexAttributeType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "type", type);

  if (!*name && semantic == VertexAttributeSemantic_Custom)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Attribute with custom semantic must have name");
    
    //проверка совместимости типа атрибута с его семантикой
    
  if (!is_compatible (semantic, type))
  {
    if (*name)
    {
      throw xtl::format_not_supported_exception (METHOD_NAME, "Semantic '%s' for attribute '%s' is not compatible with type '%s'",
        get_semantic_name (semantic), name, get_type_name (type));
    }
    else
    {
      throw xtl::format_not_supported_exception (METHOD_NAME, "Semantic '%s' is not compatible with type '%s'",
        get_semantic_name (semantic), get_type_name (type));
    }
  }

    //проверка наличия атрибута в формате

  if (*name)
  {
    if (FindAttribute (name))
      throw xtl::format_not_supported_exception (METHOD_NAME, "Attribute '%s' has been already inserted", name);
  }
  else
  {
    if (FindAttribute (semantic))
      throw xtl::format_not_supported_exception (METHOD_NAME, "Anonymous attribute with semantic '%s' has been already inserted", get_semantic_name (semantic));
  }
    
  if (impl->attributes.size () == (uint32_t)-1)
    throw xtl::format_operation_exception (METHOD_NAME, "Attributes max count exceeded");

    //добавление атрибута

  size_t old_capacity        = impl->names.Capacity (),
         old_buffer_capacity = impl->names.BufferCapacity ();  

  impl->names.Add (name);

  if (impl->names.Capacity () != old_capacity || impl->names.BufferCapacity () != old_buffer_capacity)
    impl->UpdateNames ();

  try
  {
    VertexAttribute attribute;

    attribute.name     = impl->names [impl->names.Size () - 1];
    attribute.semantic = semantic;
    attribute.type     = type;
    attribute.offset   = offset;

    impl->attributes.push_back (attribute);

    impl->need_hash_update        = true;
    impl->need_vertex_size_update = true;
  }
  catch (...)
  {
    impl->names.Remove (impl->names.Size () - 1);
    throw;
  }
  
  return (uint32_t)impl->attributes.size () - 1;
}

uint32_t VertexFormat::AddAttribute (const char* name, VertexAttributeType type, uint32_t offset)
{
  return AddAttribute (name, VertexAttributeSemantic_Custom, type, offset);
}

uint32_t VertexFormat::AddAttribute (VertexAttributeSemantic semantic, VertexAttributeType type, uint32_t offset)
{
  return AddAttribute ("", semantic, type, offset);
}

uint32_t VertexFormat::AddAttributes (const VertexFormat& format)
{
  for (VertexAttributeArray::const_iterator iter=format.impl->attributes.begin (), end=format.impl->attributes.end (); iter!=end; ++iter)
  {
    const char* name = iter->name;

    if (*name)
    {
      if (FindAttribute (name))
        throw xtl::make_argument_exception ("media::geometry::VertexFormat::AddAttributes", "name", name, "Attribute has been already inserted");
    }
    else
    {
      if (FindAttribute (iter->semantic))
        throw xtl::make_argument_exception ("media::geometry::VertexFormat::AddAttributes", "semantic", iter->semantic, "Attribute has been already inserted");
    }
  }
                            
  for (VertexAttributeArray::const_iterator iter=format.impl->attributes.begin (), end=format.impl->attributes.end (); iter!=end; ++iter)
  {
    const VertexAttribute& attribute = *iter;
    
    AddAttribute (attribute.name, attribute.semantic, attribute.type, attribute.offset);
  }

  return (uint32_t)impl->attributes.size () - 1;
}

/*
    Удаление атрибутов
*/

void VertexFormat::RemoveAttribute (uint32_t position)
{
  if (position >= impl->attributes.size ())
    return;
    
  impl->attributes.erase (impl->attributes.begin () + position);
  impl->names.Remove (position);

  if (position != impl->attributes.size ())
    impl->UpdateNames ();

  impl->need_hash_update        = true;
  impl->need_vertex_size_update = true;
}

void VertexFormat::RemoveAttribute (const char* name)
{
  const VertexAttribute* attribute = FindAttribute (name);
  
  if (!attribute)
    return;
    
  RemoveAttribute ((uint32_t)(attribute - &impl->attributes [0]));
}

void VertexFormat::RemoveAttributes (VertexAttributeSemantic semantic)
{
  const VertexAttribute* attribute = 0;

  for (;;)
  {
    attribute = FindAttribute (semantic, attribute);
  
    if (!attribute)
      return;
    
    RemoveAttribute ((uint32_t)(attribute - &impl->attributes [0]));
  }
}

void VertexFormat::RemoveAttributes (const VertexFormat& format)
{
  for (VertexAttributeArray::const_iterator iter=format.impl->attributes.begin (), end=format.impl->attributes.end (); iter!=end; ++iter)
  {
    const VertexAttribute& attribute = *iter;

    if (*attribute.name)
    {
      if (const VertexAttribute* iter = FindAttribute (attribute.name))
        RemoveAttribute ((uint32_t)(iter - &impl->attributes [0]));
    }
    else
    {
      RemoveAttributes (attribute.semantic);
    }      
  }
}

void VertexFormat::Clear ()
{
  impl->need_vertex_size_update = true;
  impl->need_hash_update        = true;

  impl->attributes.clear ();
  impl->names.Clear ();
}

/*
    Расчёт размера вершины
*/

uint32_t VertexFormat::GetMinimalVertexSize () const
{
  if (!impl->need_vertex_size_update)
    return impl->min_vertex_size;

  impl->need_vertex_size_update = false;

  if (impl->attributes.empty ())
    return impl->min_vertex_size = 0;

  uint32_t max_offset = 0;
  
  for (VertexAttributeArray::const_iterator iter=impl->attributes.begin (), end=impl->attributes.end (); iter != end; ++iter)
  {
    uint32_t offset = iter->offset + get_type_size (iter->type);

    if (offset > max_offset)
      max_offset = offset;
  }

  return impl->min_vertex_size = max_offset;
}

/*
    Расчёт хэша вершин
*/

size_t VertexFormat::Hash () const
{
  return impl->Hash ();
}

/*
   Сериализация / десериализация
*/

size_t VertexFormat::SerializationSize () const
{
  uint32_t names_data_size = 0, attributes_data_size = impl->attributes.size () * (sizeof (VertexAttribute) - offsetof(VertexAttribute, semantic));

  for (size_t i = 0, count = impl->names.Size (); i < count; i++)
  {
    names_data_size += sizeof (uint32_t) + xtl::xstrlen (impl->names [i]) + 1; //store string length, string data and null termination symbol for each name
  }

  return sizeof (uint32_t) + sizeof (uint32_t) + names_data_size + attributes_data_size + sizeof (impl->min_vertex_size);
}

size_t VertexFormat::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::VertexFormat::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_written = 0;

  uint32_t names_data_size = 0;

  if (sizeof (names_data_size) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing names data size");

  bytes_written += sizeof (names_data_size);  //skip for now, write names_data_size later

  uint32_t attributes_count = impl->names.Size ();

  if (sizeof (attributes_count) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing attributes count");

  memcpy ((char*)buffer + bytes_written, &attributes_count, sizeof (attributes_count));

  bytes_written += sizeof (attributes_count);

  for (uint32_t i = 0; i < attributes_count; i++)
  {
    uint32_t string_length = xtl::xstrlen (impl->names [i]) + 1;

    if (sizeof (string_length) + bytes_written > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing string length");

    memcpy ((char*)buffer + bytes_written, &string_length, sizeof (string_length));

    bytes_written += sizeof (string_length);

    if (string_length + bytes_written > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing string");

    memcpy ((char*)buffer + bytes_written, impl->names [i], string_length);

    bytes_written   += string_length;
    names_data_size += string_length;
  }

  memcpy (buffer, &names_data_size, sizeof (names_data_size));

  size_t attribute_size = sizeof (VertexAttribute) - offsetof(VertexAttribute, semantic);

  for (uint32_t i = 0; i < attributes_count; i++)
  {
    if (attribute_size + bytes_written > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing attribute");

    memcpy ((char*)buffer + bytes_written, &impl->attributes [i].semantic, attribute_size);

    bytes_written += attribute_size;
  }

  //update minimal vertex size before writing
  GetMinimalVertexSize ();

  if (sizeof (impl->min_vertex_size) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing min vertex size");

  memcpy ((char*)buffer + bytes_written, &impl->min_vertex_size, sizeof (impl->min_vertex_size));

  bytes_written += sizeof (impl->min_vertex_size);

  return bytes_written;
}

size_t VertexFormat::Read (const void* buffer, size_t buffer_size)
{
  size_t bytes_read = 0;

  VertexFormat::CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

VertexFormat VertexFormat::CreateFromSerializedData (const void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::VertexFormat::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  VertexFormat new_vf;

  uint32_t names_data_size = 0;

  if (sizeof (names_data_size) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading names data size");

  memcpy (&names_data_size, buffer, sizeof (names_data_size));

  bytes_read += sizeof (names_data_size);

  new_vf.impl->names.ReserveBuffer (names_data_size);

  uint32_t attributes_count;

  if (sizeof (attributes_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading attributes count");

  memcpy (&attributes_count, (char*)buffer + bytes_read, sizeof (attributes_count));

  bytes_read += sizeof (attributes_count);

  new_vf.impl->attributes.resize (attributes_count);

  for (uint32_t i = 0; i < attributes_count; i++)
  {
    uint32_t string_length;

    if (sizeof (string_length) + bytes_read > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading string length");

    memcpy (&string_length, (char*)buffer + bytes_read, sizeof (string_length));

    bytes_read += sizeof (string_length);

    if (string_length + bytes_read > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading attribute name");

    new_vf.impl->names.Add ((char*)buffer + bytes_read);

    bytes_read += string_length;
  }

  size_t attribute_read_size = sizeof (VertexAttribute) - offsetof(VertexAttribute, semantic);

  for (uint32_t i = 0; i < attributes_count; i++)
  {
    if (attribute_read_size + bytes_read > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading attribute");

    new_vf.impl->attributes [i].name = new_vf.impl->names [i];

    memcpy (&new_vf.impl->attributes [i].semantic, (char*)buffer + bytes_read, attribute_read_size);

    bytes_read += attribute_read_size;
  }

  if (sizeof (new_vf.impl->min_vertex_size) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading min vertex size");

  memcpy (&new_vf.impl->min_vertex_size, (char*)buffer + bytes_read, sizeof (new_vf.impl->min_vertex_size));

  bytes_read += sizeof (new_vf.impl->min_vertex_size);

  new_vf.impl->need_vertex_size_update = false;

  out_bytes_read = bytes_read;

  return new_vf;
}

/*
    Перегрузка операторов
*/

VertexFormat& VertexFormat::operator += (const VertexFormat& format)
{
  AddAttributes (format);
  return *this;
}

VertexFormat& VertexFormat::operator -= (const VertexFormat& format)
{
  RemoveAttributes (format);
  return *this;
}

VertexFormat VertexFormat::operator + (const VertexFormat& format) const
{
  return VertexFormat (*this) += format;
}

VertexFormat VertexFormat::operator - (const VertexFormat& format) const
{
  return VertexFormat (*this) -= format;
}

/*
    Сравнение
*/

bool VertexFormat::operator == (const VertexFormat& vf) const
{ 
  return impl->attributes.size () == vf.impl->attributes.size () && impl->Hash () == vf.impl->Hash ();
}

bool VertexFormat::operator != (const VertexFormat& vf) const
{
  return !(*this == vf);
}

/*
    Обмен
*/

void VertexFormat::Swap (VertexFormat& vf)
{
  stl::swap (impl, vf.impl);
}

namespace media
{

namespace geometry
{

void swap (VertexFormat& vf1, VertexFormat& vf2)
{
  vf1.Swap (vf2);
}

}

}
