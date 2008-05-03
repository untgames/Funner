#ifndef SG_TESTS_SHARED
#define SG_TESTS_SHARED

#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include <xtl/visitor.h>
#include <sg/node.h>
#include <sg/light.h>
#include <sg/listener.h>
#include <sg/camera.h>
#include <sg/sound_emitter.h>
#include <sg/visual_model.h>
#include <sg/scene.h>
#include <stdio.h>

using namespace scene_graph;
using namespace math;
using namespace xtl;
using namespace bound_volumes;

/*
    Тестовый объект
*/

class TestEntity: public Entity
{
  public:
    using Entity::SetBoundBox;
    using Entity::SetInfiniteBounds;
    
    static TestEntity* Create () { return new TestEntity; }
    
  protected:
    void AcceptCore (Visitor& visitor)
    {
      if (!TryAccept (*this, visitor))
        Entity::AcceptCore (visitor);
    }
};

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

inline void dump_position (Node& node)
{
  printf ("local=");
  dump   (node.Position ());
  printf (" world=");
  dump   (node.WorldPosition ());
}

inline void dump_orientation (Node& node)
{
  printf ("local=");
  dump   (node.Orientation ());
  printf (" world=");
  dump   (node.WorldOrientation ());
}

inline void dump_scale (Node& node)
{
  printf ("local=");
  dump   (node.Scale ());
  printf (" world=");
  dump   (node.WorldScale ());
}

#endif
