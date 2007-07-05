#include <stl/set>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mset1_test:\n");
  
  typedef multiset<int> mset;  
  
  mset s;
  
  printf   ("count(42) = %d\n",s.count (42));  
  s.insert (42);
  printf   ("count(42) = %d\n",s.count (42));    
  s.insert (42);  
  printf   ("count(42) = %d\n",s.count (42));      

  mset::iterator i = s.find (40);
  
  if (i == s.end()) printf ("40 Not found\n");
  else              printf ("Found %d\n",*i);
  
  i = s.find (42);

  if (i == s.end()) printf ("40 Not found\n");
  else              printf ("Found %d\n",*i);    

  printf ("Erased %d instances\n",s.erase (42));
  
  return 0;
}
