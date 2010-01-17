#include "shared.h"

const char* LIBRARY_NAME = "data/test.xrfx";

int main ()
{
  printf ("Results of xrfx_loader_test:\n");
  
  try
  {
    EffectLibrary library (LIBRARY_NAME);
    
    for (EffectLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
      dump (*iter, library.ItemId (iter));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
