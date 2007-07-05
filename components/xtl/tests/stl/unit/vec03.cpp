#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of vec03_test:\n");
  
  typedef vector<char> vec_type;

  vec_type v1; // Empty vector of characters.
  
  v1.push_back ('h');
  v1.push_back ('i');
  
  printf ("v1 = %c%c\n",v1 [0],v1 [1]);

  vec_type v2 (v1.begin(),v1.end());
  
  v2 [1] = 'o'; // Replace second character.
  
  printf ("v2 = %c%c\n",v2 [0],v2 [1]);
  printf ("(v1 == v2) = %d\n",v1==v2);
  printf ("(v1 < v2) = %d\n",v1<v2);  

  return 0;
}
