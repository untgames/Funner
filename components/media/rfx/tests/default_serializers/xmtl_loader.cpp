#include "shared.h"

const char* LIBRARY_NAME = "data/test.xmtl";

int main ()
{
  printf ("Results of xmtl_loader_test:\n");
  
  try
  {
    MaterialLibrary library (LIBRARY_NAME);
    
    for (MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
      dump (*iter, library.ItemId (iter));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
