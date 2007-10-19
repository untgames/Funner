#include <xtl/any.h>
#include <stdio.h>

using namespace xtl;

struct A
{
  virtual const char* name () const { return "class A"; }
};

struct B : A
{
  virtual const char* name () const { return "class B"; }
};

struct C : A
{
  virtual const char* name () const { return "class C"; }
};

void print (double x)
{
  printf ("%g", x);
}

void print (const A* a)
{
  printf ("static-type='class A' dynamic-type='%s'", a->name ());
}

void print (const B* a)
{
  printf ("static-type='class B' dynamic-type='%s'", a->name ());
}

void print (const C* a)
{
  printf ("static-type='class C' dynamic-type='%s'", a->name ());
}

template <class T>
void test (const char* name, const any& a)
{
  printf ("cast to '%s': ", name);

  try
  {
    print (polymorphic_any_cast<T> (a));
  }
  catch (std::exception& exception)
  {
    printf ("fail: %s", exception.what ());
  }
  
  printf ("\n");  
}

int main ()
{
  printf ("Results of any_cast_test:\n");
  
  try
  {
    typedef static_caster_factory<float, double, long double> my_factory1;
    typedef static_caster_factory<A*> my_factory2;
    typedef dynamic_caster_factory<A*, B*, C*> my_factory3;
    
    any a1 (1, my_factory1 ());
    
    test<float>  ("float", a1);
    test<double> ("double", a1);
    test<long double> ("long double", a1);
    
    B b;
    C c;
    
    any a2 (&b, my_factory2 ());

    test<A*> ("class A", a2);
    test<B*> ("class B", a2);
    test<C*> ("class C", a2);
    
    const any a3 (static_cast<A*> (&c), my_factory3 ());
    
    test<A*> ("class A", a3);
    test<B*> ("class B", a3);
    test<C*> ("class C", a3);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
