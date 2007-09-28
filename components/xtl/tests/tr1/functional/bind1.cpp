#include "test.h"

using namespace stl;

string f (const string& x)
{
  return "f(" + x + ")";
}

string g (const string& x)
{
  return "g(" + x + ")";
}

string h (const string& x, const string& y)
{
  return "h(" + x + ", " + y + ")";
}

string k ()
{
  return "k()";
}

template <class F> void test (F f)
{
  printf ("%s\n", f ("x", "y").c_str ());
}

int main()
{
  printf ("Results of bind1_test:\n");

  // compose_f_gx

  test (bind (f, bind (g, _1)));

  // compose_f_hxy

  test (bind (f, bind (h, _1, _2)));

  // compose_h_fx_gx

  test (bind (h, bind (f, _1), bind (g, _1)));

  // compose_h_fx_gy

  test (bind (h, bind (f, _1), bind (g, _2)));

  // compose_f_k

  test (bind (f, bind (k)));

  return 0;
}
