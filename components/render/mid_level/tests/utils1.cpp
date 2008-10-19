#include <cstdio>
#include <exception>

#include <render/mid_level/common.h>

using namespace render::mid_level;

int main ()
{
  printf ("Results of utils1_test:\n");
  
  try
  {  
    for (int i=0; i<PixelFormat_Num; i++)
      printf ("%s\n", get_name ((PixelFormat)i));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
