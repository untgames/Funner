#include <stl/hash_map>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of hmmap1_test:\n");

  typedef hash_multimap<char,int> mmap;

  mmap m;

  printf        ("count('X') = %lu\n",m.count ('X'));
  m.insert      (make_pair ('X',10)); // Standard way.
  printf        ("count('X') = %lu\n",m.count ('X'));
  m.insert_pair ('X',20); // Non-standard, but very convenient!
  printf        ("count('X') = %lu\n",m.count ('X'));
  m.insert_pair ('Y',32);

  for (mmap::iterator i=m.find ('X');i!=m.end ();++i)
    printf ("%c -> %d\n",i->first,i->second);

  printf ("Erased %lu items\n",m.erase ('X'));

  return 0;
}
