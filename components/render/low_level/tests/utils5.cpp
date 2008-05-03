#include "shared.h"

int main ()
{
  printf ("Results of utils5_test:\n");
  
  size_t all_flags = BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture |
                     BindFlag_RenderTarget | BindFlag_DepthStencil;
  
  for (size_t i=0; i<=all_flags; i++)
  {    
    printf ("bind_flags=%02x, semantic=%s\n", i, get_name ((BindFlag)i));
  }

  return 0;
}
