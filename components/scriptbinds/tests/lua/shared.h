#ifndef SCRIPTBINDS_TESTS_SHARED_HEADER
#define SCRIPTBINDS_TESTS_SHARED_HEADER

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <xtl/shared_ptr.h>
#include <xtl/function.h>

#include <script/bind_libraries.h>
#include <script/environment.h>
#include <script/bind.h>

#include <common/heap.h>

#include <mathlib.h>

using namespace script;
using namespace math;

inline void log_print (const char* message)
{
  printf ("%s\n", message);
}

inline void print (float x)
{
  printf ("%g", x);
}

inline void print (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

template <class T>
inline void print (const char* tag, const T& value)
{
  printf ("%s: ", tag);
  print  (value);
  printf ("\n");
}

#endif
