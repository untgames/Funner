#include <stl/bitset>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of bitset1_test:\n");

  bitset<13U> b1 = 0xFFFF, b2 = 0x1111;

  printf ("b1 size = %d\n",b1.size ());  
  printf ("b1 = %s\n",b1.to_string ().c_str ());
  printf ("b2 size = %d\n",b2.size ());    
  printf ("b2 = %s\n",b2.to_string ().c_str ());  

  b1 = b1 ^ (b2 << 2);
  
  printf ("b1 = %s\n",b1.to_string ().c_str ());  
  
  return 0;
}

