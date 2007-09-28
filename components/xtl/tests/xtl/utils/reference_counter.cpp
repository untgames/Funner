#include <stdio.h>
#include <xtl/reference_counter.h>
#include <xtl/utility>

struct A: public xtl::reference_counter, public xtl::noncopyable
{
  A () { printf ("A::A\n"); }
  ~A () { printf ("A::~A\n"); }
};

void my_deleter ()
{
  printf ("my_deleter\n");
}

int main ()
{
  printf ("Results of reference_counter_test:\n");
  
  A* p1 = new A;
  
  release (p1, &my_deleter);
  addref  (p1);  
  release (p1);

  return 0;
}
