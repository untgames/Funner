#include "shared.h"

int main ()
{
  VertexFormat vf1;
    
  vf1.AddAttribute (VertexAttributeSemantic_Position, VertexAttributeType_Float3, 2);
  vf1.AddAttribute (VertexAttributeSemantic_Normal, VertexAttributeType_Short3, 3);
  
  printf ("dump vf1\n");
  
  dump (vf1);
  
  VertexFormat vf2;
  
  vf1.AddAttribute (VertexAttributeSemantic_Color, VertexAttributeType_Float3, 4);
  
  printf ("dump vf2\n");
  
  dump (vf2);
  
  VertexFormat vf3 = vf1 + vf2;
  
  printf ("dump vf3 = vf1 + vf2\n");

  dump (vf3);
  
  printf ("dump vf4\n");
  
  VertexFormat vf4;
  
  vf4.AddAttribute (VertexAttributeSemantic_Normal, VertexAttributeType_Short3, 5);
  
  dump (vf4);
  
  printf ("dump vf5 = vf3 - vf4\n");
  
  VertexFormat vf5 = vf3 - vf4;
  
  dump (vf5);

  return 0;
}
