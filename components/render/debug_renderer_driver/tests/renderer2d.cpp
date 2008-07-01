#include "shared.h"

int main ()
{
  printf ("Results of renderer2d_test:\n");
  
  try
  {
    Test test;
    
    RendererPtr renderer (DriverManager::CreateRenderer ("debug", "renderer2d"), false);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
