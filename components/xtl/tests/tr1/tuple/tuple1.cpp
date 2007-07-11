#include "test.h"

#include <stl/string>
#include <stl/utility>

using namespace stl;

// ----------------------------------------------------------------------------
// helpers 
// ----------------------------------------------------------------------------

class A {}; 
class B {}; 
class C {};

// classes with different kinds of conversions
class AA {};
class BB : public AA {}; 
struct CC { CC() {} CC(const BB&) {} };
struct DD { operator CC() const { return CC(); }; };

// something to prevent warnings for unused variables
template<class T> void dummy(const T&) {}

// no public default constructor
class foo {
public:
  explicit foo(int v) : val(v) {}

  bool operator==(const foo& other) const  {
    return val == other.val;
  }

private:
  foo() {}
  int val;
};

// another class without a public default constructor
class no_def_constructor {
  no_def_constructor() {}
public:
  no_def_constructor(string) {}
};

// A non-copyable class 
class no_copy {
  no_copy(const no_copy&) {}
public:
  no_copy() {};
};


// ----------------------------------------------------------------------------
// Testing different element types --------------------------------------------
// ----------------------------------------------------------------------------


typedef tuple<int> t1;

typedef tuple<double&, const double&, const double, double*, const double*> t2;
typedef tuple<A, int(*)(char, int), C> t3;
typedef tuple<string, pair<A, B> > t4;
typedef tuple<A*, tuple<const A*, const B&, C>, bool, void*> t5;
typedef tuple<volatile int, const volatile char&, int(&)(float) > t6;
typedef tuple<B(A::*)(C&), A&> t7;

// -----------------------------------------------------------------------
// -tuple construction tests ---------------------------------------------
// -----------------------------------------------------------------------


no_copy y;
tuple<no_copy&> x = tuple<no_copy&>(y); // ok

char cs[10];
tuple<char(&)[10]> v2(cs);  // ok

void construction_test()
{

  // Note, the get function can be called without the tuples:: qualifier,
  // as it is lifted to namespace boost with a "using tuples::get" but
  // MSVC 6.0 just cannot find get without the namespace qualifier

  tuple<int> t1;
  TEST (get<0>(t1) == int());

  tuple<float> t2(5.5f);
  TEST (get<0>(t2) > 5.4f && get<0>(t2) < 5.6f);

  tuple<foo> t3(foo(12));
  TEST (get<0>(t3) == foo(12));

  tuple<double> t4(t2);
  TEST (get<0>(t4) > 5.4 && get<0>(t4) < 5.6);

  tuple<int, float> t5;
  TEST (get<0>(t5) == int());
  TEST (get<1>(t5) == float());

  tuple<int, float> t6(12, 5.5f);
  TEST (get<0>(t6) == 12);
  TEST (get<1>(t6) > 5.4f && get<1>(t6) < 5.6f);

  tuple<int, float> t7(t6);
  TEST (get<0>(t7) == 12);
  TEST (get<1>(t7) > 5.4f && get<1>(t7) < 5.6f);

  tuple<long, double> t8(t6);
  TEST (get<0>(t8) == 12);
  TEST (get<1>(t8) > 5.4f && get<1>(t8) < 5.6f);

  dummy(
    tuple<no_def_constructor, no_def_constructor, no_def_constructor>(
       string("Jaba"),   // ok, since the default
       string("Daba"),   // constructor is not used
       string("Doo")
    )
  );

// testing default values
  dummy(tuple<int, double>());
  dummy(tuple<int, double>(1));
  dummy(tuple<int, double>(1,3.14));


  //  dummy(tuple<double&>()); // should fail, not defaults for references
  //  dummy(tuple<const double&>()); // likewise

  double dd = 5;
  dummy(tuple<double&>(dd)); // ok

  dummy(tuple<const double&>(dd+3.14)); // ok, but dangerous

  //  dummy(tuple<double&>(dd+3.14)); // should fail,
  //                                  // temporary to non-const reference
}


// ----------------------------------------------------------------------------
// - testing element access ---------------------------------------------------
// ----------------------------------------------------------------------------

void element_access_test()
{
  double d = 2.7;
  A a;
  tuple<int, double&, const A&, int> t(1, d, a, 2);
  const tuple<int, double&, const A, int> ct = t;

  int i  = get<0>(t);
  int i2 = get<3>(t);
  
  TEST (i == 1 && i2 == 2);

  int j  = get<0>(ct);
  TEST (j == 1);
   
  get<0>(t) = 5;
   
  //  get<0>(ct) = 5; // can't assign to const

  double e = get<1>(t);
  TEST (e > 2.69 && e < 2.71);
     
  get<1>(t) = 3.14+i;
  TEST (get<1>(t) > 4.13 && get<1>(t) < 4.15);

  //  get<4>(t) = A(); // can't assign to const
  //  dummy(get<5>(ct)); // illegal index

  ++get<0>(t);
  TEST (get<0>(t) == 6);

  TEST ((is_const<tuple_element<0, tuple<int, float> >::type>::value != true));
  TEST ((is_const<tuple_element<0, const tuple<int, float> >::type>::value));
  TEST ((is_const<tuple_element<1, tuple<int, float> >::type>::value != true));
  TEST ((is_const<tuple_element<1, const tuple<int, float> >::type>::value));

  dummy(i); dummy(i2); dummy(j); dummy(e); // avoid warns for unused variables
}


// ----------------------------------------------------------------------------
// - copying tuples -----------------------------------------------------------
// ----------------------------------------------------------------------------

void copy_test()
{
  tuple<int, char> t1(4, 'a');
  tuple<int, char> t2(5, 'b');
  t2 = t1;
  TEST (get<0>(t1) == get<0>(t2));
  TEST (get<1>(t1) == get<1>(t2));

  tuple<long, string> t3(2, "a");
  t3 = t1;
  TEST ((double)get<0>(t1) == get<0>(t3));
  TEST (get<1>(t1) == get<1>(t3)[0]);

// testing copy and assignment with implicit conversions between elements
// testing tie

  tuple<char, BB*, BB, DD> t;
  tuple<int, AA*, CC, CC> a(t);
  a = t;

  int i; char c; double d;
  tie(i, c, d) = make_tuple(1, 'a', 5.5);
  
  TEST (i==1);
  TEST (c=='a');
  TEST (d>5.4 && d<5.6);
}

void mutate_test()
{
  tuple<int, float, bool, foo> t1(5, 12.2f, true, foo(4));
  get<0>(t1) = 6;
  get<1>(t1) = 2.2f;
  get<2>(t1) = false;
  get<3>(t1) = foo(5);

  TEST (get<0>(t1) == 6);
  TEST (get<1>(t1) > 2.1f && get<1>(t1) < 2.3f);
  TEST (get<2>(t1) == false);
  TEST (get<3>(t1) == foo(5));
}

// ----------------------------------------------------------------------------
// make_tuple tests -----------------------------------------------------------
// ----------------------------------------------------------------------------

void make_tuple_test()
{
  tuple<int, char> t1 = make_tuple(5, 'a');
  TEST (get<0>(t1) == 5);
  TEST (get<1>(t1) == 'a');

  tuple<int, string> t2;
  t2 = make_tuple((short int)2, string("Hi"));
  TEST (get<0>(t2) == 2);
  TEST (get<1>(t2) == "Hi");

  A a = A(); B b;
  const A ca = a;
  make_tuple(cref(a), b);
  make_tuple(ref(a), b);
  make_tuple(ref(a), cref(b));

  make_tuple(ref(ca));
   
// the result of make_tuple is assignable:
  TEST (make_tuple(2, 4, 6) == 
           (make_tuple(1, 2, 3) = make_tuple(2, 4, 6)));

  make_tuple("Donald", "Daisy"); // should work;

// You can store a reference to a function in a tuple
  tuple<void(&)()> adf(make_tuple_test);

  dummy(adf); // avoid warning for unused variable
 
// But make_tuple doesn't work 
// with function references, since it creates a const qualified function type

//   make_tuple(make_tuple_test);
  
// With function pointers, make_tuple works just fine
   make_tuple(&make_tuple_test);
      
// NOTE:
//
// wrapping it the function reference with ref helps on gcc 2.95.2.
// on edg 2.43. it results in a catastrophic error?

// make_tuple(ref(foo3));

// It seems that edg can't use implicitly the ref's conversion operator, e.g.:
// typedef void (&func_t) (void);
// func_t fref = static_cast<func_t>(ref(make_tuple_test)); // works fine 
// func_t fref = ref(make_tuple_test);                        // error

// This is probably not a very common situation, so currently
// I don't know how which compiler is right (JJ)
}

void tie_test()
{
  int a;
  char b;
  foo c(5);

  tie(a, b, c) = make_tuple(2, 'a', foo(3));
  TEST (a == 2);
  TEST (b == 'a');
  TEST (c == foo(3));

  tie(a, ignore, c) = make_tuple((short int)5, false, foo(5));
  TEST (a == 5);
  TEST (b == 'a');
  TEST (c == foo(5));

// testing assignment from pair
   int i, j; 
   tie (i, j) = make_pair(1, 2);
   TEST (i == 1 && j == 2);

   tuple<int, int, float> ta;
#ifdef E11
   ta = make_pair(1, 2); // should fail, tuple is of length 3, not 2
#endif

   dummy(ta);
}


// ----------------------------------------------------------------------------
// - testing tuple equality   -------------------------------------------------
// ----------------------------------------------------------------------------

void equality_test()
{
  tuple<int, char> t1(5, 'a');
  tuple<int, char> t2(5, 'a');
  TEST (t1 == t2);

  tuple<int, char> t3(5, 'b');
  tuple<int, char> t4(2, 'a');
  TEST (t1 != t3);
  TEST (t1 != t4);
  TEST (!(t1 != t2));
}


// ----------------------------------------------------------------------------
// - testing tuple comparisons  -----------------------------------------------
// ----------------------------------------------------------------------------

void ordering_test()
{
  tuple<int, float> t1(4, 3.3f);
  tuple<short, float> t2(5, 3.3f);
  tuple<long, double> t3(5, 4.4);
  TEST (t1 < t2);
  TEST (t1 <= t2);
  TEST (t2 > t1);
  TEST (t2 >= t1);
  TEST (t2 < t3);
  TEST (t2 <= t3);
  TEST (t3 > t2);
  TEST (t3 >= t2);
}

// ----------------------------------------------------------------------------
// - testing const tuples -----------------------------------------------------
// ----------------------------------------------------------------------------
void const_tuple_test()
{
  const tuple<int, float> t1(5, 3.3f);
  TEST (get<0>(t1) == 5);
  TEST (get<1>(t1) == 3.3f);
}

// ----------------------------------------------------------------------------
// - testing length -----------------------------------------------------------
// ----------------------------------------------------------------------------
void tuple_length_test()
{
  typedef tuple<int, float, double> t1;
  typedef tuple<> t2;

  TEST (tuple_size<t1>::value == 3);
  TEST (tuple_size<t2>::value == 0);
}

// ----------------------------------------------------------------------------
// - main ---------------------------------------------------------------------
// ----------------------------------------------------------------------------

int main ()
{
  printf ("Results of tuple1_test:\n");

  construction_test();
  element_access_test();
  copy_test();
  mutate_test();
  make_tuple_test();
  tie_test();
  equality_test();
  ordering_test();
  const_tuple_test();
  tuple_length_test();

  return 0;
}
