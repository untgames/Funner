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
  MaterialHashMap map;               //map holding materials
  uint32_t        next_material_id;  //material id for next material to add
  unsigned int    data_update_index; //current data update index

  Impl ()
    : next_material_id (0)
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
    Создание копии
*/

MaterialMap MaterialMap::Clone () const
{
  return MaterialMap (new Impl (*impl));
}

/*
    Идентификатор буфера
*/

size_t MaterialMap::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
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
