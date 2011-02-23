#include <cstdio>

#include <stl/algorithm>
#include <stl/vector>

struct B
{
  float a;
  float b;

  B (const float& in_a, const float& in_b)
    : a (in_a), b (in_b)
    {}
};

struct A
{
  B     b;
  float c [16];

  A (const B& in_b) : b (in_b) {}
};

struct a_less
{
  bool operator () (const A& a1, const A& a2) const
  {
    return a1.b.a < a2.b.a;
  }
};

int main ()
{
  printf ("Results of vec11_test:\n");

  {
    stl::vector<A> array;

    for (size_t i = 0; i < 64; i++)
    {
      B b (0.f, 1.f);

      array.insert (array.end (), &b, &b + 1);
    }

    stl::sort (array.begin (), array.end (), a_less ());
  }
  
  printf ("sort complete\n");
  fflush (stdout);

  return 0;
}
