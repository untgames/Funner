#include "shared.h"

const char* LIBRARY_PATH = "data/shaders/*";

int main ()
{
  printf ("Results of shader_loader_test:\n");
  
  try
  {
    ShaderLibrary library;
    
    library.Load (LIBRARY_PATH);
    
    for (ShaderLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
      dump (*iter, library.ItemId (iter));
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
