#include "shared.h"

void dump (VertexBuffer& vb)
{
  printf ("Vertex buffer id=%llu, source_id=%llu (%u streams, %u vertices, %u weights, vertex_size=%u)\n",
          vb.Id (), vb.SourceId (), vb.StreamsCount (),
          vb.VerticesCount (), vb.Weights ().Size (), vb.VertexSize ());
          
  for (uint32_t i=0; i<vb.StreamsCount (); i++)
  {
    printf (" stream #%u: vertices_count=%u\n", i, vb.Stream (i).Size ());
    dump (vb.Stream (i).Format ());
  }
}

int main ()
{
  printf ("Results of vertex_buffer_test:\n");
  
  VertexStream vs1 (12, make_vertex_declaration<CustomVertex> ()),
               vs2 (10, make_vertex_declaration<Vertex<Position2f> > ());
  VertexWeightStream weights (12);
  VertexBuffer vb1, vb2;

  vb1.Attach (vs1);

  VertexStream vs3 = vs2.Clone ();

  if (vs3.SourceId () == vs2.Id ())
    printf ("source id is correct after clone\n");

  vb1.Attach (vs3);
  vb1.AttachWeights (weights);
  
  printf ("Dump vb1\n");
  
  dump (vb1);
  
  printf ("Dump vb2\n");
  
  dump (vb2);  
  
  printf ("vb2 = vb1\n");
  
  vb2 = vb1;  
  
  dump (vb2);
  
  printf ("resize streams\n");
  
  vs1.Resize (9);
  vs2.Resize (8);
  
  memset (vs1.Data (), 0, vs1.Size () * vs1.VertexSize ());
  memset (vs2.Data (), 0, vs2.Size () * vs2.VertexSize ());
  memset (vs3.Data (), 0, vs3.Size () * vs3.VertexSize ());
  memset (weights.Data (), 0, weights.Size () * sizeof (VertexWeight));

  printf ("Dump vb1\n");
  
  dump (vb1);
  
  printf ("Dump vb2\n");
  
  dump (vb2);

  printf ("vb1 serialization size is %u\n", vb1.SerializationSize ());

  xtl::uninitialized_storage<char> serialization_buffer (vb1.SerializationSize ());

  size_t written_bytes = vb1.Write (serialization_buffer.data (), serialization_buffer.size ());

  printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

  size_t bytes_read;

  VertexBuffer vb3 = VertexBuffer::CreateFromSerializedData (serialization_buffer.data (), serialization_buffer.size (), bytes_read);

  printf ("Bytes read during deserialization = %u\n", bytes_read);

  printf ("deserialized vb:");

  dump (vb3);
  
  printf ("detach stream from vb1\n");
  
  vb1.Detach (1);
  
  dump (vb1);
  
  printf ("clear vb2\n");
  
  vb2.Clear ();
  
  dump (vb2);

  return 0;
}
