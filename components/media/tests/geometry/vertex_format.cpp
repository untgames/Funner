#include "shared.h"

void try_add_attribute (VertexFormat& vf, VertexAttributeSemantic semantic, VertexAttributeType type)
{
  try
  {
    vf.AddAttribute (semantic, type, 0);
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
      VertexAttribute* attribute = vf3.FindAttribute ((VertexAttributeSemantic)i);
      
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
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
