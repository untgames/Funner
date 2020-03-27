#include "shared.h"

int main ()
{
  printf ("Results of vertex_stream_test:\n");

  VertexStream vs1 (5, make_vertex_declaration<CustomVertex> ()),
               vs2 (make_vertex_declaration<CustomVertex> ());
  
  CustomVertex* verts = vs1.Data<CustomVertex> ();
  
  for (uint32_t i=0; i<vs1.Size (); i++)
  {
    verts [i].position  = float (i);
    verts [i].normal    = vec3f (1, 0, 0);
    verts [i].color     = vec4ub (i, i*2, i*3, i*4);
    verts [i].texcoord0 = vec2f (float (i), -float (i));
  }

  printf ("Dump vs1\n");

  dump (vs1);
  
  printf ("copy vs1 to vs2\n");    
  
  vs2 = vs1.Clone ();
  
  dump (vs2);

  if (vs2.SourceId () == vs1.Id ())
    printf ("source id is correct after clone\n");
  
  printf ("reserve vs1 (current capacity=%u)\n", vs1.Capacity ());
  
  vs1.Reserve (14);
  
  printf ("after vs1 reserve capacity=%u\n", vs1.Capacity ());
  
  printf ("resize vs1\n");

  vs1.Resize (6);

  CustomVertex& new_vertex = vs1.Data<CustomVertex> ()[5];

  new_vertex.position  = new_vertex.normal = 0;
  new_vertex.color     = 0;
  new_vertex.texcoord0 = 0;

  dump (vs1);
  
  printf ("instance vs2\n");

  vs2 = vs1;
  
  dump (vs2);

  printf ("vs1 serialization size is %u\n", vs1.SerializationSize ());
  printf ("vs1 serialization data size is %u\n", vs1.SerializationDataSize ());

  xtl::uninitialized_storage<char> serialization_buffer (vs1.SerializationSize ());

  size_t written_bytes = vs1.Write (serialization_buffer.data (), serialization_buffer.size ());

  printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

  size_t read_bytes = 0;

  VertexStream vs3 = VertexStream::CreateFromSerializedData (serialization_buffer.data (), serialization_buffer.size (), read_bytes);

  printf ("Bytes read during deserialization = %u\n", read_bytes);

  printf ("deserialized buffer:");

  dump (vs3);

  memset (vs3.Data (), 0, vs3.SerializationDataSize ());

  written_bytes = vs1.WriteData (serialization_buffer.data (), serialization_buffer.size ());

  printf ("Bytes written during data serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

  printf ("Bytes read during data deserialization = %u\n", vs3.ReadData (serialization_buffer.data (), serialization_buffer.size ()));

  printf ("deserialized buffer data:");

  dump (vs3);

  printf ("clear vs1 (dump vs2)\n");
  
  vs1.Clear ();
  
  dump (vs2);
  
  printf ("dump vs2 format\n");

  dump (vs2.Format (), vs2.VertexSize ());

  return 0;
}
