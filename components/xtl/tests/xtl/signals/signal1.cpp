#include "test.h"
#include <stl/functional>

using namespace stl;

template<typename T>
struct max_or_default {
  typedef T result_type;
  template<typename InputIterator>
  typename InputIterator::value_type
  operator()(InputIterator first, InputIterator last) const
  {           
    if (first == last)
      return T();

    T result = *first++;
    
    for (; first != last; ++first)
    {
      T v = *first;
      
      result = v < result ? result : v;
    }

    return result;
  }
};

struct make_int {
  make_int(int n, int cn) : N(n), CN(cn) {}

  int operator()() { return N; }
  int operator()() const { return CN; }

  int N;
  int CN;
};

template<int N>
struct make_increasing_int {
  make_increasing_int() : n(N) {}

  int operator()() const { return n++; }

  mutable int n;
};


static void
test_one_arg()
{
  signal<int (int value), max_or_default<int> > s1;

  s1.connect(negate<int>());
  s1.connect(bind1st(multiplies<int>(), 2));

  TEST (s1(1) == 2);
  TEST (s1(-1) == 1);
}

int main ()
{
  printf ("Results of signal1_test:\n");

  try
  {
    test_one_arg();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
