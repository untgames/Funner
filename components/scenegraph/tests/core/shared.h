#ifndef SG_TESTS_SHARED
#define SG_TESTS_SHARED

#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include <sg/entity.h>
#include <stdio.h>

using namespace sg;
using namespace math;
using namespace xtl;

/*
    Вспомогательные утилиты вывода состояния объекта
*/

inline void dump (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

inline void dump (const quatf& q)
{
  float pitch, yaw, roll;
  
  quat2EulerAngle (q, pitch, yaw, roll);
  
  pitch = fmod (rad2deg (pitch), 360.0f);
  yaw   = fmod (rad2deg (yaw), 360.0f);
  roll  = fmod (rad2deg (roll), 360.0f);

  printf ("[%g %g %g]", pitch, yaw, roll);
}

inline void dump_position (Entity& entity)
{
  printf ("local=");
  dump   (entity.Position ());
  printf (" world=");
  dump   (entity.WorldPosition ());
}

inline void dump_orientation (Entity& entity)
{
  printf ("local=");
  dump   (entity.Orientation ());
  printf (" world=");
  dump   (entity.WorldOrientation ());
}

inline void dump_scale (Entity& entity)
{
  printf ("local=");
  dump   (entity.Scale ());
  printf (" world=");
  dump   (entity.WorldScale ());
}

#endif
