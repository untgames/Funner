#include "shared.h"

int main ()
{
  printf ("Results of utils3_test:\n");
  
  for (int i=0; i<PixelFormat_Num; i++)
  {    
    PixelFormat format = (PixelFormat)i;
    
    printf ("get_uncompressed_format(%s)=%s\n", get_name (format), get_name (get_uncompressed_format (format)));
  }

  return 0;
}

