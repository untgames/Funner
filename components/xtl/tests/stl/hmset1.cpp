#include <stl/hash_set>
#include <stdio.h>

using namespace stl;

typedef hash_multiset<char> hmset;

int main ()
{
  printf ("Results of hmset1_test:\n");

  hmset s;

  printf   ("count(%c) = %lu\n",42,s.count (42));
  s.insert (42);
  printf   ("count(%c) = %lu\n",42,s.count (42));
  s.insert (42);
  printf   ("count(%c) = %lu\n",42,s.count (42));

  hmset::iterator i = s.find (40);

  if (i == s.end ()) printf ("40 Not found\n");
  else               printf ("Found %c\n",*i);

  i = s.find (42);

  if (i == s.end ()) printf ("Not found\n");
  else               printf ("Found %c\n",*i);

  printf ("Erased %lu instances\n",s.erase (42));

  return 0;
}
