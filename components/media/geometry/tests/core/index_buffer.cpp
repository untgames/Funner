#include "shared.h"

void dump (const IndexBuffer& ib)
{
  printf ("Index buffer id=%llu, source_id=%llu (%u indices):\n", ib.Id (), ib.SourceId(), ib.Size ());
  
  switch (ib.DataType ())
  {
    case IndexType_UInt32:
      for (size_t i=0; i<ib.Size (); i++)
        printf ("  %u\n", ib.Data<const unsigned int> ()[i]);    
      break;
    case IndexType_UInt16:
      for (size_t i=0; i<ib.Size (); i++)
        printf ("  %u\n", ib.Data<const unsigned short> ()[i]);
      break;
    case IndexType_UInt8:
      for (size_t i=0; i<ib.Size (); i++)
        printf ("  %u\n", ib.Data<const unsigned char> ()[i]);
      break;      
    default:
      printf ("  Unknown data type\n");
  }  
}

int main ()
{
  printf ("Results of index_buffer_test:\n");
  
  try
  {
    IndexBuffer ib1 (12), ib2;  
    
    for (uint32_t i=0; i<ib1.Size (); i++)
      ib1.Data<unsigned int> ()[i] = i;

    printf ("Dump ib1\n");
    dump (ib1);
    
    printf ("Dump ib2\n");
    dump (ib2);
    
    ib2.Reserve (14);
    
    printf ("ib2 capacity: %u\n", ib2.Capacity ());
    
    ib2 = ib1;
    
    printf ("ib2 = ib1\n");
    dump (ib2);
    
    printf ("clear ib2\n");
    
    ib2.Clear ();
    
    dump (ib2);
    
    printf ("instance ib2\n");
    
    ib2 = ib1;
    
    ib1.Resize (5);

    dump (ib2);
    
    printf ("copy ib2\n");
    
    ib1 = ib2.Clone ();

    ib2.Resize (4);

    dump (ib1);

    if (ib1.SourceId () == ib2.Id ())
      printf ("source id is correct after clone\n");

    printf ("ib1 serialization size is %u\n", ib1.SerializationSize ());
    printf ("ib1 serialization data size is %u\n", ib1.SerializationDataSize ());

    xtl::uninitialized_storage<char> serialization_buffer (ib1.SerializationSize ());

    size_t written_bytes = ib1.Write (serialization_buffer.data (), serialization_buffer.size ());

    printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

    size_t bytes_read;

    IndexBuffer ib3 = IndexBuffer::CreateFromSerializedData(serialization_buffer.data (), serialization_buffer.size (), bytes_read);

    printf ("Bytes read during deserialization = %u\n", bytes_read);

    printf ("deserialized buffer:");

    dump (ib3);

    memset (ib3.Data (), 0, ib3.SerializationDataSize ());

    written_bytes = ib1.WriteData (serialization_buffer.data (), serialization_buffer.size ());

    printf ("Bytes written during data serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

    printf ("Bytes read during data deserialization = %u\n", ib3.ReadData (serialization_buffer.data (), serialization_buffer.size ()));

    printf ("deserialized buffer data:");

    dump (ib3);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
