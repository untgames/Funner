#ifndef BOUND_VOLUMES_TESTS_SHARED
#define BOUND_VOLUMES_TESTS_SHARED

//тестирование компиляции кода с макросами min/max
#define min
#define max

#include <bv/axis_aligned_box.h>
#include <bv/sphere.h>
#include <stdio.h>

using namespace bound_volumes;
using namespace math;

/*
    Вывод приимтивов
*/

void dump (const vec3f& v)
{
  printf ("[%.2g %.2g %.2g]", v.x, v.y, v.z);
}

void dump (const aaboxf& box)
{
  printf ("min=");
  dump (box.minimum ());
  printf (" max=");
  dump (box.maximum ());
}

void dump (const spheref& s)
{
  printf ("center=");
  dump (s.center());
  printf (" radius=%g",s.radius());
}

#endif
