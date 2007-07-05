#include <stl/memory>
#include <stdio.h>

using namespace stl;

struct A
{
  A  () { printf ("A::A (), id=%d\n",id=count++); }
  ~A () { printf ("A::~A (), id=%d\n",id); }
  
  int id;
  static int count;
};

int A::count = 0;

int main ()
{
  printf ("Results of autoptr_test:\n");

  auto_ptr<A> p1 (new A);
  auto_ptr<const A> p2;

  p2 = p1;        
  p1 = new A;
  
  return 0;
}
