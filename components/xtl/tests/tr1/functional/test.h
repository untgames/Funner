#ifndef XTL_FUNCTIONAL_TEST_HEADER
#define XTL_FUNCTIONAL_TEST_HEADER

#include <stdio.h>
#include <xtl/memory>
#include <xtl/functional>
#include <xtl/type_traits>
#include <stl/string>
#include <stl/functional>
#include <stl/utility>

using namespace tr1;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

#endif
