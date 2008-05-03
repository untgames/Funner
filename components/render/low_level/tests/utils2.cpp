#include "shared.h"

int main ()
{
  printf ("Results of utils2_test:\n");
  
  for (int i=0; i<PixelFormat_Num; i++)
  {    
    PixelFormat format = (PixelFormat)i;
    
    printf ("%s: is_compressed=%d is_uncompressed=%d is_depth_stencil=%d is_color=%d\n", get_name ((PixelFormat)i), 
            is_compressed (format), is_uncompressed (format), is_depth_stencil (format), is_color (format));
  }

  return 0;
}

