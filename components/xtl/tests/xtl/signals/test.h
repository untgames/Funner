#include <cstdio>

#include <stl/auto_ptr.h>

#include <xtl/signal.h>
#include <xtl/functional>
#include <xtl/trackable_ptr.h>

using namespace xtl;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);
