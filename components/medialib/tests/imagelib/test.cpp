#include <stdio.h>
#include <media/image.h>

using namespace medialib;
using namespace common;

int main ()
{
  try
  {
    ImageSystem::RegisterSaveCodec ("tiff", &Image::DefaultSaver);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  
  printf ("Ok!\n");
  
  return 0;
}
