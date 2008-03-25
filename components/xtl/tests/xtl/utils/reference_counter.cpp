#include <stdio.h>
#include <xtl/reference_counter.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/utility>

struct A: public xtl::reference_counter, public xtl::noncopyable
{
  A () { printf ("A::A\n"); }
  ~A () { printf ("A::~A\n"); }
};

void my_deleter (const xtl::intrusive_ptr<A>&)
{
  printf ("my_deleter\n");
}

int main ()
{
  printf ("Results of reference_counter_test:\n");
  
  xtl::intrusive_ptr<A> p1 (new A, false);

  release (p1, &my_deleter);
  addref  (*p1);

  return 0;
}
