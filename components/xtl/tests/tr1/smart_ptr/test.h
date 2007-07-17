#include <stdio.h>
#include <tr1/shared_ptr.h>
#include <tr1/intrusive_ptr.h>

using namespace tr1;

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

