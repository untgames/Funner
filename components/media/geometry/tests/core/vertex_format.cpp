#include "shared.h"

void try_add_attribute (VertexFormat& vf, VertexAttributeSemantic semantic, VertexAttributeType type)
{
  try
  {
    static const char* custom_name = "custom";

    vf.AddAttribute (semantic == VertexAttributeSemantic_Custom ? custom_name : "", semantic, type, 0);
  }
  catch (std::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of vertex_format_test:\n");
  
  try
  {  
    VertexFormat vf1;
    
    vf1.AddAttribute (VertexAttributeSemantic_Position, VertexAttributeType_Float3, 2);
    vf1.AddAttribute (VertexAttributeSemantic_Normal, VertexAttributeType_Short3, 3);
    
    printf ("dump vf1\n");
    dump (vf1);

    VertexFormat vf2 = vf1;
    
    vf2.AddAttribute (VertexAttributeSemantic_Color, VertexAttributeType_UByte4, 4);

    printf ("dump vf2\n");
        
    dump (vf2);    
    
    VertexFormat vf3 = vf2;
    
    printf ("remove attribute by index\n");

    vf2.RemoveAttribute (1);

    dump (vf2);    

    printf ("remove attribute by semantic\n");
    
    vf2.RemoveAttribute (VertexAttributeSemantic_Position);
    
    dump (vf2);

    printf ("vf2 = vf1\n");

    vf2 = vf1;

    dump (vf2);

    printf ("vf1 == vf2: %d\n", vf1 == vf2);
    printf ("vf1 != vf2: %d\n", vf1 != vf2);
    
    printf ("clear vf1\n");
    
    vf1.Clear ();

    printf ("vf1 == vf2: %d\n", vf1 == vf2);
    printf ("vf1 != vf2: %d\n", vf1 != vf2);
    
    printf ("dump vf3\n");            
    
    dump (vf3);

    printf ("minimal_vertex_size: %u\n", vf2.GetMinimalVertexSize ());                
    
    for (size_t i=0; i<VertexAttributeSemantic_Num; i++)
    {
      const VertexAttribute* attribute = vf3.FindAttribute ((VertexAttributeSemantic)i);
      
      if (attribute)
      {
        printf ("attribute with semantic '%s' found at position %u\n",
                get_semantic_name ((VertexAttributeSemantic)i), attribute - vf3.Attributes ());
      }
      else
      {
        printf ("attribute with semantic '%s' not found\n", get_semantic_name ((VertexAttributeSemantic)i));
      }
    }

    for (size_t i=0; i<VertexAttributeType_Num; i++)
      for (size_t j=0; j<VertexAttributeSemantic_Num; j++)
      {
        try_add_attribute (vf1, (VertexAttributeSemantic)j, (VertexAttributeType)i);

        vf1.Clear ();        
      }

    VertexFormat vf4;

    vf4.AddAttribute (VertexAttributeSemantic_Position, VertexAttributeType_Float3, 2);
    vf4.AddAttribute ("normal_name", VertexAttributeSemantic_Normal, VertexAttributeType_Short3, 3);
    vf4.AddAttribute ("color_name", VertexAttributeSemantic_Color, VertexAttributeType_UByte4, 4);

    printf ("dump vf4\n");

    dump (vf4);

    printf ("vf4 serialization size is %u\n", vf4.SerializationSize ());

    xtl::uninitialized_storage<char> serialization_buffer (vf4.SerializationSize ());

    size_t written_bytes = vf4.Write (serialization_buffer.data (), serialization_buffer.size ());

    printf ("Bytes written during serialization = %u, data hash = %x\n", written_bytes, common::crc32 (serialization_buffer.data (), written_bytes));

    size_t bytes_read;

    VertexFormat vf_deserialized = VertexFormat::CreateFromSerializedData (serialization_buffer.data (), serialization_buffer.size (), bytes_read);

    printf ("Bytes read during deserialization = %u\n", bytes_read);

    printf ("deserialized vertex format:\n");

    dump (vf_deserialized);

  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
