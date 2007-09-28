#include <stdio.h>
#include <xtl/token_iterator.h>

void print (const char* s)
{
  printf ("value='%s'\n", s);
}

void print (int v)
{
  printf ("value=%d\n", v);
}

void print (double v)
{
  printf ("value=%g\n", v);
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
  printf ("Results of token_iterator_test:\n");
  
  const char* tokens [] = {"1", "hello", "3.14"};
  xtl::io::token_iterator<const char*> iter = xtl::io::make_token_iterator (tokens, tokens + sizeof (tokens)/sizeof(*tokens));  
  
  test<int> (iter);
  test<int> (iter);
  test<const char*> (iter);
  test<float> (iter);
  test<float> (iter);
  
  printf ("empty: %d\n", iter.empty ());
  printf ("iter == 0: %d\n", iter == 0);
  printf ("available: %d\n", iter.available ());

  return 0;
}
