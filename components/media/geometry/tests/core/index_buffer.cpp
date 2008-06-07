#include "shared.h"

void dump (const IndexBuffer& ib)
{
  printf ("Index buffer (%u indices):\n", ib.Size ());
  
  for (size_t i=0; i<ib.Size (); i++)
    printf ("  %u\n", ib.Data ()[i]);
}

int main ()
{
  printf ("Results of index_buffer_test:\n");
  
  try
  {
    IndexBuffer ib1 (12), ib2;  
    
    for (size_t i=0; i<ib1.Size (); i++)
      ib1.Data ()[i] = i;
    
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
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
