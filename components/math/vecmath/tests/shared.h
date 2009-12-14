#ifndef MATHLIB_VECMATH_TESTS_SHARED_HEADER
#define MATHLIB_VECMATH_TESTS_SHARED_HEADER

#include <cstdio>

#include <math/angle.h>
#include <math/matrix.h>
#include <math/quat.h>
#include <math/utility.h>
#include <math/vector.h>

using namespace math;

void dump (float x)
{
  static const float EPS = 0.001f;

  if (fabs (x) < EPS)
    x = 0.0f;

  printf ("%.3f", x);
}

void dump (bool x)
{
  printf (x ? "true" : "false");
}

template <class T>
void dump (const angle<T>& ang)
{
  dump (degree (ang));
  printf (" deg");
}

template <class T, unsigned int Size>
void dump (const vector<T, Size>& v)
{
  printf ("[");

  for (unsigned int i=0; i<Size; i++)
  {
    if (i)
      printf (", ");
  
    dump (v [i]);
  }
  
  printf ("]");
}

template <class T>
void dump (const quat<T>& q)
{
  printf ("[");

  for (int i=0; i<3; i++)
  {
    if (i)
      printf (", ");
  
    dump (q [i]);
  }
  
  printf ("] ");
  dump   (q [3]);
}

template <class T, unsigned int Size>
void dump (const matrix<T, Size>& m)
{
  for (unsigned int i=0; i<Size; i++)
  {
    printf ("\n");
    dump (m [i]);
  }
}

template <class T>
void dump (const char* message, const T& x)
{
  printf ("%s: ", message);
  dump (x);
  printf ("\n");
}

#endif
