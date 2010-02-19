#include <stdio.h>
#include <xtl/lock_ptr.h>
#include <xtl/stride_ptr.h>
#include <xtl/intrusive_ptr.h>

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

using namespace xtl;
