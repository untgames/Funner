#include <stdio.h>
#include <stl/memory>
#include <xtl/shared_ptr.h>
#include <xtl/intrusive_ptr.h>

using namespace tr1;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

