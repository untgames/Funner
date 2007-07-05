#include <stl/vector>
#include <stdio.h>

using namespace stl;

static void print (const char* name,vector<double>& v)
{
  printf ("%s = ",name);
  for (int i=0;i<(int)v.size ();i++)
    printf ("%g ",v [i]);
  printf ("\n");
}

int main ()
{
  printf ("Results of vec02_test:\n");

  vector<double> v1, v2; // Empty vector of doubles.
    
  v1.push_back (32.1);
  v1.push_back (40.5);  
  v2.push_back (3.56);
  print      ("v1",v1);
  print      ("v2",v2);  
  v1.swap    (v2); // Swap the vector's contents.  
  print      ("v1",v1);
  print      ("v2",v2);  
  
  v2 = v1; // Assign one vector to another.
  
  print ("v2",v2);
  
  return 0;
}
