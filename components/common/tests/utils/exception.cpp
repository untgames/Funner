#include <common/exception.h>
#include <stdio.h>

using namespace common;

#define TEST(X)                                   \
  try                                             \
  {                                               \
    try                                           \
    {                                             \
      X;                                          \
    }                                             \
    catch (Exception& exception)                  \
    {                                             \
      exception.Touch ("some_function");          \
      throw;                                      \
    }                                             \
  }                                               \
  catch (std::exception& exception)               \
  {                                               \
    printf ("exception: %s\n",exception.what ()); \
  }                                               \

int main ()
{
  printf ("Results of exception_test:\n");

  TEST (raise_invalid_argument ("foo","x"));
  TEST (raise_invalid_argument ("foo","x","hello world"));
  TEST (raise_invalid_argument ("foo","x",-1));
  TEST (raise_invalid_argument ("foo","x",1));
  TEST (raise_invalid_argument ("foo","x",3.14f));
  TEST (raise_out_of_range ("foo","x"));
  TEST (raise_out_of_range ("foo","x",0,1,2));
  TEST (raise_out_of_range ("foo","x",0,-2,-1));
  TEST (raise_out_of_range ("foo","x",3.14f,0.0f,1.0f));
  TEST (raise_out_of_range ("foo","x",5,4));
  TEST (raise_null_argument ("foo","ptr"));
  TEST (raise_not_implemented ("foo"));

  return 0;
}
