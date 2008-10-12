#include <stl/map>
#include <stdio.h>

using namespace stl;

int main ()
{
  typedef multimap<char,int> mmap;

  printf ("Results of mmap1_test:\n");

  mmap m;

  printf        ("count('X') = %lu\n",m.count ('X'));
  m.insert      (make_pair ('X',10)); //standart way
  printf        ("count('X') = %lu\n",m.count ('X'));
  m.insert_pair ('X',20); // Non-standard, but very convenient!
  printf        ("count('X') = %lu\n",m.count ('X'));
  m.insert_pair ('Y',32);

  for (mmap::iterator i=m.find ('X');i != m.end();i++)
    printf ("%c -> %d\n",i->first,i->second);

  printf ("Erased %lu items\n",m.erase ('X'));

  return 0;
}
