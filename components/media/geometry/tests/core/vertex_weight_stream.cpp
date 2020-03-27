#include "shared.h"

void dump (const VertexWeightStream& s)
{
  printf ("Vertex weight stream id=%llu, source_id=%llu (%u weights):\n", s.Id (), s.SourceId (), s.Size ());
  
  const VertexWeight* weight = s.Data ();
  
  for (size_t i=0; i<s.Size (); i++, weight++)
    printf ("  joint=%u weight=%g\n", weight->joint_index, weight->joint_weight);
}

int main ()
{
  printf ("Results of vertex_weight_stream_test:\n");
  
  try
  {
    VertexWeightStream s1 (12), s2;  
    
    for (uint32_t i=0; i<s1.Size (); i++)
      s1.Data ()[i] = VertexWeight (i, 0.5f);
 
    printf ("Dump s1\n");
    dump (s1);
    
    printf ("Dump s2\n");
    dump (s2);
    
    s2.Reserve (14);
    
    printf ("s2 capacity: %u\n", s2.Capacity ());
    
    s2 = s1.Clone ();
    
    printf ("copy s1 to s2\n");
    dump (s2);

    if (s2.SourceId () == s1.Id ())
      printf ("source id is correct after clone\n");
    
    printf ("clear s2\n");
    
    s2.Clear ();
    
    dump (s2);
    
    printf ("instance s2\n");
    
    s2 = s1;
    
    s1.Resize (5);

    dump (s2);    

    printf ("s1 serialization size is %u\n", s1.SerializationSize ());
    printf ("s1 serialization data size is %u\n", s1.SerializationDataSize ());

    xtl::uninitialized_storage<char> serialization_buffer (s1.SerializationSize ());

    size_t written_bytes = s1.Write (serialization_buffer.data (), serialization_buffer.size ());

    printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

    size_t bytes_read;

    VertexWeightStream s3 = VertexWeightStream::CreateFromSerializedData (serialization_buffer.data (), serialization_buffer.size (), bytes_read);

    printf ("Bytes read during deserialization = %u\n", bytes_read);

    printf ("deserialized buffer:");

    dump (s3);

    memset (s3.Data (), 0, s3.SerializationDataSize ());

    written_bytes = s1.WriteData (serialization_buffer.data (), serialization_buffer.size ());

    printf ("Bytes written during data serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

    printf ("Bytes read during data deserialization = %u\n", s3.ReadData (serialization_buffer.data (), serialization_buffer.size ()));

    printf ("deserialized buffer data:");

    dump (s3);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
