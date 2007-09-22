#ifndef XTL_RANGE_TESTS_HEADER
#define XTL_RANGE_TESTS_HEADER

#include <xtl/range_traits.h>
#include <xtl/iterator_range.h>
#include <xtl/sub_range.h>
#include <xtl/type_traits>
#include <stl/vector>
#include <stl/string>
#include <stl/algorithm>
#include <stdio.h>

using namespace xtl;
using namespace stl;
using namespace xtl::type_traits;

#define CHECK_TYPE(EXPECTED_TYPE, TYPE_EXPRESSION) \
{ \
  if (is_same<TYPE_EXPRESSION, EXPECTED_TYPE>::value) \
    printf ("expression %s=%s\n", #TYPE_EXPRESSION, #EXPECTED_TYPE); \
  else \
    printf ("error: expression %s=%s must be %s\n", #TYPE_EXPRESSION, typeid (TYPE_EXPRESSION).name (), #EXPECTED_TYPE); \
}

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

#endif
