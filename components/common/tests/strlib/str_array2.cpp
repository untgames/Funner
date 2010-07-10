#include <cstdio>
#include <exception>

#include <common/hash.h>
#include <common/string.h>

using namespace common;

void dump (const char* title, const StringArray& sa)
{
  printf ("%s (size=%u, buffer_size=%u, hash=%08x): {", title, sa.Size (), sa.BufferSize (), crc32 (sa.Buffer (), sa.BufferSize ()));

  for (size_t i=0; i<sa.Size (); i++)
  {
    if (i)
      printf (", ");

    printf ("%s", sa [i]);
  }

  printf ("}\n");
}

int main ()
{
  printf ("Results of str_array1_test:\n");
  
  try
  {
    StringArray sa1, sa2;
    
    sa1 += "1";
    sa1 += "2";
    sa1 += "3";
    sa1 += "4";
    sa1 += "5";    
    
    sa2 += "1";
    sa2 += "6";
    sa2 += "5";    
    
    sa1.Remove (1, 2);
        
    sa2.Set (1, "4");
    
    dump ("sa1", sa1);
    dump ("sa2", sa2);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
}
