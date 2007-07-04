#include <common/hash.h>

namespace common
{

//алгоритм взят из библиотеки mhash_0.9

size_t adler32 (const void* buf,size_t len,size_t hash_value)
{
  size_t s1 = hash_value & 0x0000FFFF, 
         s2 = (hash_value >> 16) & 0x0000FFFF;
               
  for (const unsigned char* p = (const unsigned char*)buf;len--;p++)
  {
    s1 += *p;
    
    if (s1 >= 65521)  /* using modulo took about 7 times longer on my CPU! */
      s1 -= 65521;  /* WARNING: it's meant to be >= 65521, not just > ! */
      
    s2 += s1;
    
    if (s2 >= 65521)  /* same warning applies here, too */
      s2 -= 65521;
  }
  
  return (s2 << 16) + s1;
}

}
