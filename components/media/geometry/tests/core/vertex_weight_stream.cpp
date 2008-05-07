#include "shared.h"

void dump (const VertexWeightStream& s)
{
  printf ("Vertex weight stream (%u weights):\n", s.Size ());
  
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
    
    for (size_t i=0; i<s1.Size (); i++)
      s1.Data ()[i] = VertexWeight (i, 0.5f);
 
    printf ("Dump s1\n");
    dump (s1);
    
    printf ("Dump s2\n");
    dump (s2);
    
    s2.Reserve (14);
    
    printf ("s2 capacity: %u\n", s2.Capacity ());
    
    s2 = clone (s1, CloneMode_Copy);
    
    printf ("copy s1 to s2\n");
    dump (s2);
    
    printf ("clear s2\n");
    
    s2.Clear ();
    
    dump (s2);
    
    printf ("instance s2\n");
    
    s2 = s1;
    
    s1.Resize (5);

    dump (s2);    
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
