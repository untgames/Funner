#include <stdio.h>
#include <xtl/any.h>
#include <stl/string>
#include <stl/utility>
#include <stl/memory>

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

#define TEST_THROW(X,Y) try { (X); printf ("executed '%s'\n", #X); } catch (Y&) { printf ("exception '%s' accepted\n", #Y); } \
  catch (...) { printf ("wrong exception. must be '%s'\n", #Y); }

using namespace xtl;
