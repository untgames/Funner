#include <stdio.h>
#include <xtl/signal.h>
#include <xtl/functional>

using namespace xtl;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);
