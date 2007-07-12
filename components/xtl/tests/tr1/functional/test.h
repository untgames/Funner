#include <stdio.h>
#include <tr1/memory>
#include <tr1/functional>
#include <tr1/type_traits>

using namespace tr1;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

