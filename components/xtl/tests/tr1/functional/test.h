#include <stdio.h>
#include <xtl/memory>
#include <xtl/functional>
#include <xtl/type_traits>

using namespace tr1;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

