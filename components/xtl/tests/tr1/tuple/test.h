#include <stdio.h>
#include <tr1/tuple>
#include <tr1/type_traits>
#include <tr1/functional>

using namespace tr1;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);
