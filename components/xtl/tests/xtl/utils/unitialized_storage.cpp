#include <cstdio>
#include <xtl/uninitialized_storage.h>

typedef xtl::uninitialized_storage<int> my_storage;

void print (const char* title, const my_storage& s)
{
  printf ("%s: size=%lu, capacity=%lu, empty=%s, content={", title, s.size (), s.capacity (), s.empty () ? "true" : "false");

  for (size_t i=0; i<s.size (); i++)
  {
    if (i)
      printf (", ");

    printf ("%d", s.data ()[i]);
  }
    
  printf ("}\n");
}

int main ()
{
  printf ("Results of uninitialized_storage_test:\n");  
  
  my_storage s1 (10);
  
  for (size_t i=0; i<s1.size (); i++)
    s1.data ()[i] = i;
    
  print ("s1", s1);
  
  printf ("Reserve 100 elements\n");

  s1.reserve (100);
  
  print ("s1", s1);
  
  printf ("Copy to s2\n");
  
  my_storage s2 = s1;
  
  print ("s1", s1);
  print ("s2", s2);
  
  printf ("Remove elements\n");
  
  s1.resize (5);
  
  print ("s1", s1);
  
  printf ("Swap s1 and s2\n");
  
  swap (s1, s2);
  
  print ("s1", s1);
  print ("s2", s2);    
   
  printf ("Assign s1 to s2\n");
  
  s2 = s1;
  
  print ("s1", s1);
  print ("s2", s2);  
  
  printf ("Clear\n");
  
  s2.resize (0);
  
  print ("s2", s2);

  return 0;
}
