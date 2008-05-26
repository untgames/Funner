#include "shared.h"

struct A: public dynamic_cast_root
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

namespace test_namespace
{

struct X
{
  X() {}
  X(int in_value) : value (in_value) {}
  
  int value;
};

struct Y : X
{
  Y () {}
  Y(int value) : X (value) {}
};

void to_value (const stl::string& buffer, X& x)
{
  xtl::to_value (buffer, x.value);
}

void to_string (stl::string& buffer, X& x)
{
  xtl::to_string (buffer, x.value);
}

}

template <class T>
void print (const volatile T* value)
{
  print (*value);
}

void print (const volatile A& a)
{
  printf ("static-type='class A' dynamic-type='%s'", const_cast<const A&> (a).name ());
}

void print (const volatile B& b)
{
  printf ("static-type='class B' dynamic-type='%s'", const_cast<const B&> (b).name ());
}

void print (const volatile C& c)
{
  printf ("static-type='class C' dynamic-type='%s'", const_cast<const C&> (c).name ());
}

void print (const volatile test_namespace::X& x)
{
  printf ("static-type='class X' value=%u", x.value);
}

void print (const volatile test_namespace::Y& y)
{
  printf ("static-type='class Y' value=%u", y.value);
}

template <class T>
void print (const T* value)
{
  print (static_cast<const volatile T*> (value));
}

void print (const A& a)
{
  print (static_cast<const volatile A&> (a));
}

void print (const B& b)
{
  print (static_cast<const volatile B&> (b));
}

void print (const C& c)
{
  print (static_cast<const volatile C&> (c));
}

void print (const test_namespace::X& x)
{
  print (static_cast<const volatile test_namespace::X&> (x));
}

void print (const test_namespace::Y& y)
{
  print (static_cast<const volatile test_namespace::Y&> (y));
}

template <class T>
void test (const char* name, const any& a)
{
  printf ("cast to '%s': ", name);

  try
  {
    print (any_multicast<T> (a));
  }
  catch (bad_any_cast& exception)
  {
//    printf ("fail: %s (at cast from '%s' to '%s')", exception.what (), exception.source_type ().name (), exception.target_type ().name ());
    printf ("fail: %s", exception.what ());
  }  
  catch (std::exception& exception)
  {
    printf ("fail: %s", exception.what ());
  }
  
  printf ("\n");  
}

int main ()
{
  using namespace test_namespace;

  printf ("Results of any_cast_test:\n");
  
  try
  {
    any a1 (1);

    printf ("check lexical_cast\n");
    
    test<float>  ("float", a1);
    test<double> ("double", a1);
    test<long double> ("long double", a1);
    
    B b;
    C c;    
    
    printf ("check dynamic any_multicast (source type - class B)\n");
    
    any a2 (&b);

    test<A*> ("class A", a2);
    test<B*> ("class B", a2);
    test<C*> ("class C", a2);
    
    printf ("check dynamic any_multicast (source type - class C, casted to A)\n");
    
    const any a3 (static_cast<A*> (&c));
    
    test<A*> ("class A", a3);
    test<B*> ("class B", a3);    
    test<C*> ("class C", a3);
    test<const A&> ("const class A&", a3);
    test<const B&> ("const class B&", a3);    
    test<const C&> ("const class C&", a3);    
    test<X*> ("class X*", a3);
    
    printf ("check dynamic any_multicast (source type - class C, casted to const A)\n");
    
    const any a4 (static_cast<const A*> (&c));    
    
    test<A*> ("class A", a4);
    test<B*> ("class B", a4);    
    test<C*> ("class C", a4);    
    test<const A*> ("const class A", a4);
    test<const B*> ("const class B", a4);    
    test<const C*> ("const class C", a4);
    test<volatile A*> ("volatile class A", a4);
    test<volatile B*> ("volatile class B", a4);    
    test<volatile C*> ("volatile class C", a4);    
    test<const volatile A*> ("const volatile class A", a4);
    test<const volatile B*> ("const volatile class B", a4);
    test<const volatile C*> ("const volatile class C", a4);
    
    printf ("check dynamic any_multicast (source type - stl::auto_ptr<const class C>)\n");
  
    stl::auto_ptr<const C> a5_ptr (new C);

    const any a5 (a5_ptr);

    test<A*> ("class A", a5);
    test<B*> ("class B", a5);    
    test<C*> ("class C", a5);    
    test<const A*> ("const class A", a5);
    test<const B*> ("const class B", a5);    
    test<const C*> ("const class C", a5);
    test<volatile A*> ("volatile class A", a5);
    test<volatile B*> ("volatile class B", a5);    
    test<volatile C*> ("volatile class C", a5);    
    test<const volatile A*> ("const volatile class A", a5);
    test<const volatile B*> ("const volatile class B", a5);
    test<const volatile C*> ("const volatile class C", a5);    
    
    printf ("check complex-lexical_cast\n");
    
    Y y (123);
    any a6 (xtl::ref (y));
    
    test<Y&> ("class Y&", a6);
    test<Y> ("class Y", a6);
    test<const Y> ("const class Y", a6);
    test<X> ("class X", a6);
    test<X&> ("class X&", a6);
    
    printf ("check set_content\n");    

    to_value ("321", a6);
    
    printf ("y.value=%d\n", y.value);
    
    a6 = xtl::cref (y);
    
    to_value ("4321", a6);
    
    printf ("y.value=%d\n", y.value); //must be unreachable
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
