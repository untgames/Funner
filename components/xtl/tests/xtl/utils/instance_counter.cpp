#include <cstdio>

#include <xtl/instance_counter.h>

using namespace xtl;

struct A: public instance_counter<A> {};
struct B: public A, public instance_counter<B> {};

void dump ()
{
  printf ("instance groups:\n");

  for (instance_counter_group_iterator i; i; ++i)
  {
    const char* type_name = i->type ().name ();
    
    if (&typeid (A) == &i->type ()) type_name = "A";
    if (&typeid (B) == &i->type ()) type_name = "B";    
    
    printf ("  type=%s, count=%u\n", type_name, i->size ());
  }
}

int main ()
{
  printf ("Results of instance_counter_test:\n");
  
  {
    A a [3];
    
    dump ();
  
    {
      B b [2];
      
      dump ();
    }
    
    dump ();    
  }

  return 0;
}
