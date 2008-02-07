#include "shared.h"

int main ()
{
  printf ("Results of utils1_test:\n");
  
  for (int i=0; i<PixelFormat_Num; i++)
  {
    static const size_t WIDTH = 16, HEIGHT = 4;    
    
    PixelFormat format = (PixelFormat)i;
    
    printf ("%x%x@%s: size=%u uncompressed_size=%u\n", WIDTH, HEIGHT, get_name (format), get_image_size (WIDTH, HEIGHT, format),
            get_uncompressed_image_size (WIDTH, HEIGHT, format));
  }

  return 0;
}
