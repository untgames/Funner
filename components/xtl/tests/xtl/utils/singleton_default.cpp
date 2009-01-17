#include <cstdio>

#include <xtl/singleton_default.h>

using namespace xtl;

template <int id> struct A
{
  A ()  { printf ("A<%d>::A()\n", id); }
  ~A () { printf ("A<%d>::~A()\n", id); }
};

int main ()
{
  printf ("Results of singleton_default_test:\n");
  
  typedef singleton_default<A<0> > non_used_singleton;
  
  singleton_default<A<1> >::instance ();
  singleton_default<A<3> >::instance ();
  singleton_default<A<2> >::instance ();
  singleton_default<A<4>, false >::instance ();  

  return 0;
}
