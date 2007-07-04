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
      exception.Raise ("some_function");          \
    }                                             \
  }                                               \
  catch (std::exception& exception)               \
  {                                               \
    printf ("exception: %s\n",exception.what ()); \
  }                                               \

int main ()
{
  printf ("Results of exception_test:\n");

  TEST (RaiseInvalidArgument ("foo","x"));
  TEST (RaiseInvalidArgument ("foo","x","hello world"));
  TEST (RaiseInvalidArgument ("foo","x",-1));
  TEST (RaiseInvalidArgument ("foo","x",1));
  TEST (RaiseInvalidArgument ("foo","x",3.14f));
  TEST (RaiseOutOfRange ("foo","x"));
  TEST (RaiseOutOfRange ("foo","x",0,1,2));
  TEST (RaiseOutOfRange ("foo","x",0,-2,-1));
  TEST (RaiseOutOfRange ("foo","x",3.14f,0.0f,1.0f));
  TEST (RaiseOutOfRange ("foo","x",5,4));
  TEST (RaiseNullArgument ("foo","ptr"));
  TEST (RaiseNotImplemented ("foo"));

  return 0;
}
