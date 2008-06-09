#include "shared.h"

#define TEST(X)                                   \
  try                                             \
  {                                               \
    try                                           \
    {                                             \
      throw X;                                    \
    }                                             \
    catch (exception& exception)                  \
    {                                             \
      exception.touch ("some_function");          \
      throw;                                      \
    }                                             \
  }                                               \
  catch (std::exception& exception)               \
  {                                               \
    printf ("exception: %s\n", exception.what ());\
  }                                               \

int main ()
{
  printf ("Results of common_exceptions_test:\n");

  TEST (make_argument_exception ("foo", "x"));
  TEST (make_argument_exception ("foo", "x", "hello world"));
  TEST (make_argument_exception ("foo", "x", "hello world", "Some reason"));  
  TEST (make_argument_exception ("foo", "x", -1));
  TEST (make_argument_exception ("foo", "x", 1));
  TEST (make_argument_exception ("foo", "x", 3.14f));
  TEST (make_range_exception ("foo", "x"));
  TEST (make_range_exception ("foo", "x", 0, 1, 2));
  TEST (make_range_exception ("foo", "x", 0, -2, -1));
  TEST (make_range_exception ("foo", "x", 3.14f, 0.0f, 1.0f));
  TEST (make_range_exception ("foo", "x", 5, 4));
  TEST (make_null_argument_exception ("foo", "ptr"));
  TEST (make_not_implemented_exception ("foo"));
  TEST (format_operation_exception ("foo", "Invalid operation '%s'", "some_operation"));
  TEST (make_unsupported_platform_exception ("foo"));
  TEST (make_unsupported_platform_exception ("foo", "MyPlatform"));

  return 0;
}
