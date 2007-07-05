#include <stl/set>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

inline bool less_than    (int a,int b) { return a < b; }
inline bool greater_than (int a,int b) { return a > b; }

int main ()
{
  printf ("Results of mset5_test:\n");
  
  typedef pointer_to_binary_function<int,int,bool> fn_type;
  typedef multiset<int,fn_type> mset;  
  
  int array [] = {3,6,1,9}; 

  mset s1 (array,array+4,less_than);
  mset::iterator i;
  
  printf ("Using less_than: \n");
  
  for (i=s1.begin();i!=s1.end();i++) printf ("%d\n",*i);
    
  mset s2 (array,array+4,greater_than);
  
  printf ("Using greater_than: \n");  
  
  for (i=s2.begin();i!=s2.end();i++) printf ("%d\n",*i);

  return 0;
}
