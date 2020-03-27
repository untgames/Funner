#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

namespace
{

typedef stl::hash_map<uint32_t, stl::string> MaterialHashMap;

struct MaterialHashMapSelector
{
  const uint32_t& operator () (MaterialHashMap::value_type& value) const { return value.first; }
};

}

/*
    MaterialMap implementation
*/

struct MaterialMap::Impl: public xtl::reference_counter, public xtl::trackable
{
  object_id_t     id;                //identifier of map
  object_id_t     source_id;         //identifier of map from which this map was cloned / deserialized
  MaterialHashMap map;               //map holding materials
  uint32_t        next_material_id;  //material id for next material to add
  unsigned int    data_update_index; //current data update index

  Impl ()
    : id (IdPool::AllocateId (ObjectType_MaterialMap))
    , source_id (id)
    , next_material_id (0)
    , data_update_index (0)
    {}

  Impl (const Impl& source)
    : id (IdPool::AllocateId (ObjectType_MaterialMap))
    , source_id (source.source_id)
    , map (source.map)
    , next_material_id (source.next_material_id)
    , data_update_index (0)
    {}
};

/*
    Конструкторы / деструктор
*/

MaterialMap::MaterialMap ()
  : impl (new Impl, false)
  {}

MaterialMap::MaterialMap (Impl* in_impl)
  : impl (in_impl, false)
  {}

MaterialMap::MaterialMap (const MaterialMap& map)
  : impl (map.impl)
  {}
  
MaterialMap::~MaterialMap ()
{
}

/*
    Присваивание
*/

MaterialMap& MaterialMap::operator = (const MaterialMap& map)
{
  impl = map.impl;

  return *this;
}

/*
    Clone
*/

MaterialMap MaterialMap::Clone () const
{
  return MaterialMap (new Impl (*impl));
}

/*
    Map id (unique)
*/

object_id_t MaterialMap::Id () const
{
  return impl->id;
}

/*
   Identifier of map from which this map was cloned / deserialized
*/

object_id_t MaterialMap::SourceId () const
{
  return impl->source_id;
}

/*
   Getting elements
*/

const char* MaterialMap::MaterialName (uint32_t id) const
{
  MaterialHashMap::iterator iter = impl->map.find (id);

  if (iter == impl->map.end ())
    throw xtl::make_argument_exception ("media::geometry::MaterialMap::MaterialName", "id", (size_t)id, "Unknown material id");

  return iter->second.c_str ();
}

int32_t MaterialMap::FindMaterialId (const char* name) const
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::geometry::MaterialMap::FindMaterialId", "name");

  for (MaterialHashMap::iterator iter = impl->map.begin (), end = impl->map.end (); iter != end; ++iter)
    if (iter->second == name)
      return iter->first;

  return -1;
}

MaterialMap::Iterator MaterialMap::CreateIterator () const
{
  return MaterialMap::Iterator (impl->map.begin (), impl->map.begin (), impl->map.end (), MaterialHashMapSelector ());
}

/*
   Adding elements
*/

uint32_t MaterialMap::SetMaterial (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::geometry::MaterialMap::SetMaterial (const char*)", "name");

  uint32_t material_id = impl->next_material_id;

  //Check if material id value reached last possible value, if so - start from beginning.
  for (MaterialHashMap::iterator end = impl->map.end (); impl->map.find (material_id) != end || material_id == INVALID_MATERIAL_ID; material_id++)
  {
    //search for next free id
    //ignore possible infinite loop if all ids are already added
  }

  impl->map.insert_pair (material_id, name);

  impl->next_material_id = material_id + 1;

  impl->data_update_index++;

  return material_id;
}

void MaterialMap::SetMaterial (uint32_t id, const char* name)
{
  static const char* METHOD_NAME = "media::geometry::MaterialMap::SetMaterial (uint32_t, const char*)";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (id == INVALID_MATERIAL_ID)
    throw xtl::make_argument_exception (METHOD_NAME, "id", (size_t)id, "This id is reserved for internal use");

  impl->map [id] = name;

  impl->data_update_index++;
}

/*
    Removing elements
*/

void MaterialMap::RemoveMaterial (const char* name)
{
  if (!name)
    return;

  for (MaterialHashMap::iterator iter = impl->map.begin (), end = impl->map.end (); iter != end;)
  {
    MaterialHashMap::iterator next = iter;

    ++next;

    if (iter->second == name)
    {
      impl->map.erase (iter);
      impl->data_update_index++;
    }

    iter = next;
  }
}

void MaterialMap::RemoveMaterial (uint32_t id)
{
  if (impl->map.erase (id))
    impl->data_update_index++;
}

void MaterialMap::RemoveMaterial (MaterialMap::Iterator iter)
{
  impl->map.erase (*iter.target<MaterialHashMap::iterator> ());
  impl->data_update_index++;
}

void MaterialMap::Clear ()
{
  impl->map.clear ();
  impl->data_update_index++;
}

/*
   Current update index
*/

unsigned int MaterialMap::CurrentDataUpdateIndex () const
{
  return impl->data_update_index;
}

/*
   Serialization / deserialization
*/

size_t MaterialMap::SerializationSize () const
{
  uint32_t materials_data_size = impl->map.size () * sizeof (uint32_t) * 2; //materials data size will contain for each material: material id (uint32_t), size of material name (uint32_t), material name

  for (MaterialHashMap::iterator iter = impl->map.begin (), end = impl->map.end (); iter != end; ++iter)
  {
    materials_data_size += iter->second.size ();
  }

  return sizeof (impl->source_id) + sizeof (uint32_t) + materials_data_size;
}

size_t MaterialMap::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::MaterialMap::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (buffer_size < SerializationSize ())
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing data");

  size_t bytes_written = 0;

  memcpy (buffer, &impl->source_id, sizeof (impl->source_id));

  bytes_written += sizeof (impl->source_id);

  uint32_t materials_count = impl->map.size ();

  memcpy ((char*)buffer + bytes_written, &materials_count, sizeof (materials_count));

  bytes_written += sizeof (materials_count);

  for (MaterialHashMap::iterator iter = impl->map.begin (), end = impl->map.end (); iter != end; ++iter)
  {
    memcpy ((char*)buffer + bytes_written, &iter->first, sizeof (iter->first));

    bytes_written += sizeof (iter->first);

    uint32_t material_name_size = iter->second.size ();

    memcpy ((char*)buffer + bytes_written, &material_name_size, sizeof (material_name_size));

    bytes_written += sizeof (material_name_size);

    memcpy ((char*)buffer + bytes_written, iter->second.data (), material_name_size);

    bytes_written += material_name_size;
  }

  return bytes_written;
}

size_t MaterialMap::Read (const void* buffer, size_t buffer_size)
{
  size_t bytes_read;

  CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

MaterialMap MaterialMap::CreateFromSerializedData (const void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::MaterialMap::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  MaterialMap new_material_map;

  if (sizeof (new_material_map.impl->source_id) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading source id");

  memcpy (&new_material_map.impl->source_id, buffer, sizeof (new_material_map.impl->source_id));

  bytes_read += sizeof (new_material_map.impl->source_id);

  uint32_t materials_count;

  if (sizeof (materials_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading materials count");

  memcpy (&materials_count, (char*)buffer + bytes_read, sizeof (materials_count));

  bytes_read += sizeof (materials_count);

  for (uint32_t i = 0; i < materials_count; i++)
  {
    uint32_t material_id;

    if (sizeof (material_id) + bytes_read > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading materials id");

    memcpy (&material_id, (char*)buffer + bytes_read, sizeof (material_id));

    bytes_read += sizeof (material_id);

    uint32_t material_name_size;

    if (sizeof (material_name_size) + bytes_read > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading materials name size");

    memcpy (&material_name_size, (char*)buffer + bytes_read, sizeof (material_name_size));

    bytes_read += sizeof (material_name_size);

    stl::string material_name;

    if (material_name_size + bytes_read > buffer_size)
      throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading materials name");

    material_name.fast_resize (material_name_size);

    memcpy (&material_name [0], (char*)buffer + bytes_read, material_name_size);

    bytes_read += material_name_size;

    new_material_map.impl->map [material_id] = material_name;
  }

  out_bytes_read = bytes_read;

  return new_material_map;
}

/*
   Destroy notifier object
*/

xtl::trackable& MaterialMap::Trackable () const
{
  return *impl;
}

/*
    Swap
*/

void MaterialMap::Swap (MaterialMap& map)
{
  impl.swap (map.impl);
}

namespace media
{

namespace geometry
{

xtl::trackable& get_trackable (const MaterialMap& map)
{
  return map.Trackable ();
}

void swap (MaterialMap& map1, MaterialMap& map2)
{
  map1.Swap (map2);
}

}

}
