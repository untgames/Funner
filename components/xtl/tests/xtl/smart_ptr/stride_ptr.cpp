#include "shared.h"

int main ()
{
  printf ("Results of stride_ptr_test:\n");
  
  typedef stride_ptr<int> my_ptr;
  
  int x [10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  
  my_ptr p, p0 (x, sizeof (int) * 2), p1 = p0, p2 = p1 + 5;
  
  for (;p1<p2; ++p1)
    printf ("%d ", *p1);
    
  printf ("\n");
  
  TEST (!p);
  TEST (p2);
  TEST (p1 == p2);
  TEST (p0 != p2);
  TEST (p0 <  p2);
  TEST (p2 >  p0);
  TEST (p0 <= p2);
  TEST (p2 >= p0);
  TEST (p1 <= p2);
  TEST (p1 >= p2);
  TEST (p1.get () == p2);
  TEST (p0.get () != p2);
  TEST (p0.get () <  p2);
  TEST (p2.get () >  p0);
  TEST (p0.get () <= p2);
  TEST (p2.get () >= p0);
  TEST (p1.get () <= p2);
  TEST (p1.get () >= p2);
  TEST (p1 == p2.get ());
  TEST (p0 != p2.get ());
  TEST (p0 <  p2.get ());
  TEST (p2 >  p0.get ());
  TEST (p0 <= p2.get ());
  TEST (p2 >= p0.get ());
  TEST (p1 <= p2.get ());
  TEST (p1 >= p2.get ());
  TEST (p1 - 5 == p0);
  TEST (p0 + 5 == p2);
  TEST (p0 [3] == 6);
  TEST ((p1 = p0) == p0);
  TEST (p1.equal (p0));

  return 0;
}
