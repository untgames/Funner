#include <cstdio>
#include <exception>

#include <common/string.h>

using namespace common;

void dump (const char* title, const StringArray& sa, bool pointers=false)
{
  printf ("%s (size=%u, empty=%s, capacity=%u, buffer_size=%u): {", title, sa.Size (), sa.IsEmpty () ? "true" : "false",
    sa.Capacity (), sa.BufferCapacity ());
    
  for (size_t i=0; i<sa.Size (); i++)
  {
    if (i)
      printf (", ");
      
    printf ("%s", pointers ? sa.Data ()[i] : sa [i]);
  }
  
  printf ("}\n");
}

int main ()
{
  printf ("Results of str_array2_test:\n");
  
  try
  {
    StringArray sa;
    
    sa += "AAAAAA";
    sa += "0";
    
    sa.Set (0, "AA");
    sa.Set (1, "0");
    
    dump ("result", sa);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
}
