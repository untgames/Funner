#include "test.h"

using namespace stl;

int global_int;

struct write_five_obj { void operator()() const { global_int = 5; } };
struct write_three_obj { int operator()() const { global_int = 3; return 7; }};
static void write_five() { global_int = 5; }
static void write_three() { global_int = 3; }
struct generate_five_obj { int operator()() const { return 5; } };
struct generate_three_obj { int operator()() const { return 3; } };
static int generate_five() { return 5; }
static int generate_three() { return 3; }
static string identity_str(const string& s) { return s; }
static string string_cat(const string& s1, const string& s2) { return s1+s2; }
static int sum_ints(int x, int y) { return x+y; }

struct write_const_1_nonconst_2
{
  void operator()() { global_int = 2; }
  void operator()() const { global_int = 1; }
};

struct add_to_obj
{
  add_to_obj(int v) : value(v) {}

  int operator()(int x) const { return value + x; }

  int value;
};

static void
test_zero_args()
{
  typedef function<void ()> func_void_type;

  write_five_obj five;
  write_three_obj three;

  // Default construction
  func_void_type v1;
  TEST (v1.empty());

  // Assignment to an empty function
  v1 = five;
  TEST (v1 != null);

  // Invocation of a function
  global_int = 0;
  v1();
  TEST (global_int == 5);

  // clear() method
  v1.clear();
  TEST (v1 == null);

  // Assignment to an empty function
  v1 = three;
  TEST (!v1.empty());

  // Invocation and self-assignment
  global_int = 0;
  v1 = v1;
  v1();
  TEST (global_int == 3);

  // Assignment to a non-empty function
  v1 = five;

  // Invocation and self-assignment
  global_int = 0;
  v1 = (v1);
  v1();
  TEST (global_int == 5);

  // clear
  v1 = null;
  TEST (null == v1);

  // Assignment to an empty function from a free function
  v1 = & write_five;
  TEST (null != v1);

  // Invocation
  global_int = 0;
  v1();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v1 = & write_three;
  TEST (!v1.empty());

  // Invocation
  global_int = 0;
  v1();
  TEST (global_int == 3);

  // Assignment
  v1 = five;
  TEST (!v1.empty());

  // Invocation
  global_int = 0;
  v1();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v1 = &write_three;
  TEST (!v1.empty());

  // Invocation
  global_int = 0;
  v1();
  TEST (global_int == 3);

  // Construction from another function (that is empty)
  v1.clear();
  func_void_type v2(v1);
  TEST (!v2? true : false);

  // Assignment to an empty function
  v2 = three;
  TEST (!v2.empty());

  // Invocation
  global_int = 0;
  v2();
  TEST (global_int == 3);

  // Assignment to a non-empty function
  v2 = (five);

  // Invocation
  global_int = 0;
  v2();
  TEST (global_int == 5);

  v2.clear();
  TEST (v2.empty());

  // Assignment to an empty function from a free function
  v2 = (& write_five);
  TEST (v2? true : false);

  // Invocation
  global_int = 0;
  v2();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v2 = & write_three;
  TEST (!v2.empty());

  // Invocation
  global_int = 0;
  v2();
  TEST (global_int == 3);

  // Swapping
  v1 = five;
  swap(v1, v2);
  v2();
  TEST (global_int == 5);
  v1();
  TEST (global_int == 3);
  swap(v1, v2);
  v1.clear();

  // Assignment
  v2 = five;
  TEST (!v2.empty());

  // Invocation
  global_int = 0;
  v2();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v2 = &write_three;
  TEST (!v2.empty());

  // Invocation
  global_int = 0;
  v2();
  TEST (global_int == 3);

  // Assignment to a function from an empty function
  v2 = v1;
  TEST (v2.empty());

  // Assignment to a function from a function with a functor
  v1 = three;
  v2 = v1;
  TEST (!v1.empty());
  TEST (!v2.empty());

  // Invocation
  global_int = 0;
  v1();
  TEST (global_int == 3);
  global_int = 0;
  v2();
  TEST (global_int == 3);

  // Assign to a function from a function with a function
  v2 = & write_five;
  v1 = v2;
  TEST (!v1.empty());
  TEST (!v2.empty());
  global_int = 0;
  v1();
  TEST (global_int == 5);
  global_int = 0;
  v2();
  TEST (global_int == 5);

  // Construct a function given another function containing a function
  func_void_type v3(v1);

  // Invocation of a function
  global_int = 0;
  v3();
  TEST (global_int == 5);

  // clear() method
  v3.clear();
  TEST (!v3? true : false);

  // Assignment to an empty function
  v3 = three;
  TEST (!v3.empty());

  // Invocation
  global_int = 0;
  v3();
  TEST (global_int == 3);

  // Assignment to a non-empty function
  v3 = five;

  // Invocation
  global_int = 0;
  v3();
  TEST (global_int == 5);

  // clear()
  v3.clear();
  TEST (v3.empty());

  // Assignment to an empty function from a free function
  v3 = &write_five;
  TEST (!v3.empty());

  // Invocation
  global_int = 0;
  v3();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v3 = &write_three;
  TEST (!v3.empty());

  // Invocation
  global_int = 0;
  v3();
  TEST (global_int == 3);

  // Assignment
  v3 = five;
  TEST (!v3.empty());

  // Invocation
  global_int = 0;
  v3();
  TEST (global_int == 5);

  // Construction of a function from a function containing a functor
  func_void_type v4(v3);

  // Invocation of a function
  global_int = 0;
  v4();
  TEST (global_int == 5);

  // clear() method
  v4.clear();
  TEST (v4.empty());

  // Assignment to an empty function
  v4 = three;
  TEST (!v4.empty());

  // Invocation
  global_int = 0;
  v4();
  TEST (global_int == 3);

  // Assignment to a non-empty function
  v4 = five;

  // Invocation
  global_int = 0;
  v4();
  TEST (global_int == 5);

  // clear()
  v4.clear();
  TEST (v4.empty());

  // Assignment to an empty function from a free function
  v4 = &write_five;
  TEST (!v4.empty());

  // Invocation
  global_int = 0;
  v4();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v4 = &write_three;
  TEST (!v4.empty());

  // Invocation
  global_int = 0;
  v4();
  TEST (global_int == 3);

  // Assignment
  v4 = five;
  TEST (!v4.empty());

  // Invocation
  global_int = 0;
  v4();
  TEST (global_int == 5);

  // Construction of a function from a functor
  func_void_type v5(five);

  // Invocation of a function
  global_int = 0;
  v5();
  TEST (global_int == 5);

  // clear() method
  v5.clear();
  TEST (v5.empty());

  // Assignment to an empty function
  v5 = three;
  TEST (!v5.empty());

  // Invocation
  global_int = 0;
  v5();
  TEST (global_int == 3);

  // Assignment to a non-empty function
  v5 = five;

  // Invocation
  global_int = 0;
  v5();
  TEST (global_int == 5);

  // clear()
  v5.clear();
  TEST (v5.empty());

  // Assignment to an empty function from a free function
  v5 = &write_five;
  TEST (!v5.empty());

  // Invocation
  global_int = 0;
  v5();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v5 = &write_three;
  TEST (!v5.empty());

  // Invocation
  global_int = 0;
  v5();
  TEST (global_int == 3);

  // Assignment
  v5 = five;
  TEST (!v5.empty());

  // Invocation
  global_int = 0;
  v5();
  TEST (global_int == 5);

  // Construction of a function from a function
  func_void_type v6(&write_five);

  // Invocation of a function
  global_int = 0;
  v6();
  TEST (global_int == 5);

  // clear() method
  v6.clear();
  TEST (v6.empty());

  // Assignment to an empty function
  v6 = three;
  TEST (!v6.empty());

  // Invocation
  global_int = 0;
  v6();
  TEST (global_int == 3);

  // Assignment to a non-empty function
  v6 = five;

  // Invocation
  global_int = 0;
  v6();
  TEST (global_int == 5);

  // clear()
  v6.clear();
  TEST (v6.empty());

  // Assignment to an empty function from a free function
  v6 = &write_five;
  TEST (!v6.empty());

  // Invocation
  global_int = 0;
  v6();
  TEST (global_int == 5);

  // Assignment to a non-empty function from a free function
  v6 = &write_three;
  TEST (!v6.empty());

  // Invocation
  global_int = 0;
  v6();
  TEST (global_int == 3);

  // Assignment
  v6 = five;
  TEST (!v6.empty());

  // Invocation
  global_int = 0;
  v6();
  TEST (global_int == 5);

  // Const vs. non-const
  write_const_1_nonconst_2 one_or_two;
  const function<void ()> v7(one_or_two);
  function<void ()> v8(one_or_two);

  global_int = 0;
  v7();
  TEST (global_int == 2);

  global_int = 0;
  v8();
  TEST (global_int == 2);

  // Test construction from 0 and comparison to 0
  func_void_type v9(null);
  TEST (v9 == null);
  TEST (null == v9);

  // Test return values
  typedef function<int ()> func_int_type;
  generate_five_obj gen_five;
  generate_three_obj gen_three;

  func_int_type i0(gen_five);

  TEST (i0() == 5);
  i0 = gen_three;
  TEST (i0() == 3);
  i0 = &generate_five;
  TEST (i0() == 5);
  i0 = &generate_three;
  TEST (i0() == 3);
  TEST (i0? true : false);
  i0.clear();
  TEST (!i0? true : false);

  // Test return values with compatible types
  typedef function<long ()> func_long_type;
  func_long_type i1(gen_five);

  TEST (i1() == 5);
  i1 = gen_three;
  TEST (i1() == 3);
  i1 = &generate_five;
  TEST (i1() == 5);
  i1 = &generate_three;
  TEST (i1() == 3);
  TEST (i1? true : false);
  i1.clear();
  TEST (!i1? true : false);
}

static void
test_one_arg()
{
  negate<int> neg;

  function<int (int)> f1(neg);
  TEST (f1(5) == -5);

  function<string (string)> id(&identity_str);
  TEST (id("str") == "str");

  function<string (const char*)> id2(&identity_str);
  TEST (id2("foo") == "foo");

  add_to_obj add_to(5);
  function<int (int)> f2(add_to);
  TEST (f2(3) == 8);

  const function<int (int)> cf2(add_to);
  TEST (cf2(3) == 8);
}

static void
test_two_args()
{
  function<string (const string&, const string&)> cat(&string_cat);
  TEST (cat("str", "ing") == "string");

  function<int (short, short)> sum(&sum_ints);
  TEST (sum(2, 3) == 5);
}

static void
test_emptiness()
{
  function<float ()> f1;
  TEST (f1.empty());

  function<float ()> f2;
  f2 = f1;
  TEST (f2.empty());

  function<double ()> f3;
  f3 = f2;
  TEST (f3.empty());
}

struct X {
  X(int v) : value(v) {}

  int twice() const { return 2*value; }
  int plus(int v) { return value + v; }

  int value;
};

static void
test_member_functions()
{
  function<int (X*)> f1(&X::twice);

  X one(1);
  X five(5);

  TEST (f1(&one) == 2);
  TEST (f1(&five) == 10);

  function<int (X*)> f1_2;
  f1_2 = &X::twice;

  TEST (f1_2(&one) == 2);
  TEST (f1_2(&five) == 10);

  function<int (X&, int)> f2(&X::plus);
  TEST (f2(one, 3) == 4);
  TEST (f2(five, 4) == 9);
}

struct add_with_throw_on_copy {
  int operator()(int x, int y) const { return x+y; }

  add_with_throw_on_copy() {}

  add_with_throw_on_copy(const add_with_throw_on_copy&)
  {
    throw runtime_error("But this CAN'T throw");
  }

  add_with_throw_on_copy& operator=(const add_with_throw_on_copy&)
  {
    throw runtime_error("But this CAN'T throw");
  }
};

static void
test_ref()
{
  add_with_throw_on_copy atc;
  try {
    function<int (int, int)> f(ref(atc));
    TEST (f(1, 3) == 4);
  }
  catch(runtime_error e) {
    printf ("error: Nonthrowing constructor threw an exception\n");
  }
}

static void test_exception()
{
  function<int (int, int)> f;
  try {
    f(5, 4);
    TEST (false);
  }
  catch(bad_function_call) {
    // okay
  }
}

typedef function< void * (void * reader) > reader_type;
typedef pair<int, reader_type> mapped_type;

static void test_implicit()
{
  mapped_type m;
  m = mapped_type();
}

static void test_call_obj(function<int (int, int)> f)
{
  TEST (!f.empty());
}

static void test_call_cref(const function<int (int, int)>& f)
{
  TEST (!f.empty());
}

static void test_call()
{
  test_call_obj(plus<int>());
  test_call_cref(plus<int>());
}

int main()
{
  printf ("Results of function_test:\n");

  test_zero_args();
  test_one_arg();
  test_two_args();
  test_emptiness();
  test_member_functions();
  test_ref();
  test_exception();
  test_implicit();
  test_call();

  return 0;
}
