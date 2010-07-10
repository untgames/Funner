#include <cstdio>
#include <exception>

#include <common/string.h>

using namespace common;

void dump (const char* title, const StringArray& sa, bool pointers=false)
{
  printf ("%s (size=%u, empty=%s, capacity=%u, buffer_capacity=%u): {", title, sa.Size (), sa.IsEmpty () ? "true" : "false",
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
  printf ("Results of str_array1_test:\n");
  
  try
  {
    StringArray sa1;
    
    dump ("default constructor", sa1);
    
    printf ("data: %s\n", sa1.Data () ? "not-null" : "null");
    
    sa1 += "1";
    sa1 += "2";
    sa1 += "3";
    
    dump ("after add", sa1);
    
    sa1.Data ();

    sa1.Reserve (120);
    sa1.ReserveBuffer (1024);
    
    dump ("after reserve", sa1, true);
    
    sa1 += "4";
    
    sa1.Remove (1, 2);    

    dump ("after remove", sa1, true);    

    StringArray sa2 = sa1 + "5";

    dump ("s1 after operator +", sa1);
    dump ("s2 after operator +", sa2);
    
    sa1.Clear ();
    
    dump ("s1 after clear", sa1);
    
    sa1.Swap (sa2);
    
    dump ("s1 after swap", sa1);
    dump ("s2 after swap", sa2);    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
}
