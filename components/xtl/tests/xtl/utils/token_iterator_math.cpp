#include <stdio.h>
#include <math/vector.h>
#include <math/matrix.h>
#include <math/quat.h>
#include <xtl/token_iterator.h>

void print (const char* s)
{
  printf ("'%s'", s);
}

void print (int v)
{
  printf ("%d", v);
}

void print (double v)
{
  printf ("%g", v);
}

template <class T, size_t size>
void print (const math::vector<T, size>& v)
{
  printf ("value=[");

  for (size_t i=0; i<size; i++)
  {
    printf (" ");
    print  (v [i]);
  }

  printf (" ]\n");
}

template <class T, size_t size>
void print (const math::matrix<T, size>& v)
{
  printf ("value=[");

  for (size_t j=0; j<size; j++)
  {
    printf (" [");

    for (size_t i=0; i<size; i++)
    {
      printf (" ");
      print (v [j][i]);
    }

    printf (" ]");
  }

  printf (" ]\n");
}

template <class T>
void print (const math::quat<T>& v)
{
  printf ("value=[");

  for (size_t i=0; i<4; i++)
  {
    printf (" ");
    print  (v [i]);
  }

  printf (" ]\n");
}

template <class T, class Token, class BaseIter>
void test (xtl::io::token_iterator<Token, BaseIter>& iter)
{
  T value;

  if (!read (iter, value))
  {
    printf ("failed read next token\n");
  }
  else print (value);
}

int main ()
{
  printf ("Results of token_iterator_math_test:\n");

  const char* tokens [] = {"1", "2", "3.14", "4", "5", "6.2", "7", "9.2", "10", "11", "12", "13"};
  xtl::io::token_iterator<const char*> iter = xtl::io::make_token_iterator (tokens, tokens + sizeof (tokens)/sizeof(*tokens));

  test<math::vec3f> (iter);
  test<math::mat2i> (iter);
  test<math::quatd> (iter);
  test<math::vec3f> (iter);

  printf ("empty: %d\n", iter.empty ());
  printf ("iter == 0: %d\n", iter == 0);
  printf ("available: %lu\n", iter.available ());

  return 0;
}
