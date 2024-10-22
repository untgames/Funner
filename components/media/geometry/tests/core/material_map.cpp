#include "shared.h"

void dump (const char* name, const MaterialMap& map)
{
  printf ("Material map id=%llu, source_id = %llu, name='%s':\n", map.Id (), map.SourceId (), name);
  
  for (MaterialMap::Iterator iter = map.CreateIterator (); iter; ++iter)
  {
    printf ("  %u ('%s')\n", *iter, map.MaterialName (*iter));
  }
}

int main ()
{
  printf ("Results of material_map_test:\n");
  
  try
  {
    MaterialMap map1, map2;
    
    dump ("map1", map1);

    printf ("Set 'material1' %u\n", map1.SetMaterial ("material1"));
    printf ("Set 'material2' %u\n", map1.SetMaterial ("material2"));

    map1.SetMaterial (2, "material3");
    
    printf ("Set 'material4' %u\n", map1.SetMaterial ("material4"));

    printf ("Find material id 'material2' = %u\n", map1.FindMaterialId ("material2"));

    printf ("map2 = map1\n");

    map2 = map1;
    
    dump ("map2", map2);
    
    printf ("clear map2\n");
    
    map2.Clear ();
    
    dump ("map1", map1);
    dump ("map2", map2);
    
    printf ("Before remove\n");

    printf ("Set 'material1' %u\n", map1.SetMaterial ("material1"));
    printf ("Set 'material2' %u\n", map1.SetMaterial ("material2"));

    map1.SetMaterial (2, "material3");

    printf ("Set 'material4' %u\n", map1.SetMaterial ("material4"));
    printf ("Set 'material5' %u\n", map1.SetMaterial ("material5"));
    printf ("Set 'material6' %u\n", map1.SetMaterial ("material6"));
    printf ("Set 'material7' %u\n", map1.SetMaterial ("material7"));

    map1.SetMaterial (uint32_t (-67), "big_index_material");

    dump ("map1", map1);

    printf ("Big index material is %u\n", (uint32_t)map1.FindMaterialId ("big_index_material"));

    map1.RemoveMaterial (map1.CreateIterator ());
    map1.RemoveMaterial ("material3");
    map1.RemoveMaterial ("material333");
    map1.RemoveMaterial (5);
    map1.RemoveMaterial (100);

    printf ("After remove\n");

    dump ("map1", map1);

    printf ("clone map1\n");
    
    map2 = map1.Clone ();

    dump ("map1", map1);
    dump ("map2", map2);

    if (map2.SourceId () == map1.Id ())
      printf ("source id is correct after clone\n");

    printf ("Clear map2\n");

    map2.Clear ();

    dump ("map1", map1);
    dump ("map2", map2);

    printf ("map1 serialization size is %u\n", map1.SerializationSize ());

    xtl::uninitialized_storage<char> serialization_buffer (map1.SerializationSize ());

    size_t written_bytes = map1.Write (serialization_buffer.data (), serialization_buffer.size ());

    printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

    size_t bytes_read;

    MaterialMap map3 = MaterialMap::CreateFromSerializedData (serialization_buffer.data (), serialization_buffer.size (), bytes_read);

    printf ("Bytes read during deserialization = %u\n", bytes_read);

    printf ("deserialized map:");

    dump ("map3", map3);

    map1.SetMaterial (uint32_t (-1), "invalid_index_material");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
