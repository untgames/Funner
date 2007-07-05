#include <stl/map>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mmap2_test:\n"); 

  typedef multimap<int,char> mmap;  
  typedef pair<int,char> pair_type;

  pair_type array [] = {
    make_pair (3,'c'),
    make_pair (6,'f'),
    make_pair (1,'a'),
    make_pair (2,'b'),
    make_pair (3,'x'),
    make_pair (6,'f') 
  };

  mmap m (array,array+6);
  
  mmap::iterator i = m.lower_bound (3);
  
  printf ("lower bound:\n%d -> %c\n",i->first,i->second);

  i = m.upper_bound (3);
  
  printf ("upper bound:\n%d -> %c\n",i->first,i->second);  
  
  return 0;
}
