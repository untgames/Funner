#include <stl/algorithm>
#include <stl/memory>
#include <stdio.h>

using namespace stl;

class X
{
  public:
    X  (int _i=0) : i (_i) {}
    ~X () {}
    
    operator int() const { return i; }
 
  private:
    int i;
};

int main (int, char**)
{
  printf ("Results of rawiter_test:\n");

  allocator<X> a;
  
  X *p = a.allocate (5), *save_p = p; 
  
  raw_storage_iterator<X*> r (p);
  
  int i;
  
  for (i=0;i<5;i++) *r++ = X (i);
  for (i=0;i<5;i++) printf ("%d\n",(int)*p++);
  
  a.deallocate (save_p,5);

  return 0;
}
