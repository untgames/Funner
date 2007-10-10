#include <stdio.h>
#include <common/exception.h>
#include <media/image.h>
#include <xtl/functional>

using namespace media;
using namespace common;

const char* file_name = "data/pic1.jpg.cubemap";

int main ()
{
  try
  {
    printf ("Results of cubemax loader test:\n");

    Image image (file_name);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }
  
  return 0;
}
