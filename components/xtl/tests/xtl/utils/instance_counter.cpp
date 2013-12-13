#include <cstdio>

#include <xtl/stat_counter.h>

using namespace xtl;

struct group1;
struct group2;

struct A: public instance_counter<A, group1> {};
struct C: public instance_counter<C, group2> {};
struct B: public A, public instance_counter<B, group1> {};

const char* get_type_name (xtl::type<A>)      { return "A"; }
const char* get_type_name (xtl::type<B>)      { return "B"; }
const char* get_type_name (xtl::type<C>)      { return "C"; }
const char* get_type_name (xtl::type<group1>) { return "group1"; }
const char* get_type_name (xtl::type<group2>) { return "group2"; }

void dump ()
{
  printf ("counters:\n");

  for (stat_counter_iterator i; i; ++i)
  {    
    printf ("  group=%s, name=%s, value=%u\n", i->group_name (), i->name (), i->value ());
  }
}

int main ()
{
  printf ("Results of instance_counter_test:\n");
  
  {
    A a [3];
    
    (void)a;

    dump ();
  
    {
      C c;
      B b [2];

      (void)b;
      
      dump ();
    }
    
    dump ();    
  }

  return 0;
}
