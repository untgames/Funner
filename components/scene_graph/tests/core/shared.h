#ifndef SG_TESTS_SHARED
#define SG_TESTS_SHARED

#include <stdio.h>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/visitor.h>

#include <stl/string>

#include <common/hash.h>
#include <common/log.h>
#include <common/utf_converter.h>
#include <common/property_map.h>
#include <common/property_binding_map.h>

#include <sg/camera.h>
#include <sg/controller.h>
#include <sg/height_map.h>
#include <sg/input.h>
#include <sg/light.h>
#include <sg/listener.h>
#include <sg/mesh.h>
#include <sg/node.h>
#include <sg/node_array.h>
#include <sg/page_curl.h>
#include <sg/particle_emitter.h>
#include <sg/physics.h>
#include <sg/scene.h>
#include <sg/sound_emitter.h>
#include <sg/sprite.h>
#include <sg/text_line.h>

using namespace scene_graph;
using namespace scene_graph::controllers;
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
    
    typedef xtl::com_ptr<TestEntity> Pointer;
    
    static Pointer Create () { return Pointer (new TestEntity, false); }
    
  protected:
    void AcceptCore (Visitor& visitor)
    {
      if (!TryAccept (*this, visitor))
        Entity::AcceptCore (visitor);
    }
};

float prepare_float (float v)
{
  if (fabs (v) < 0.001f)
    return 0.f;

  return v;
}

/*
    Вспомогательные утилиты вывода состояния объекта
*/

inline void dump (const vec3f& v)
{
  printf ("[%+.2f %+.2f %+.2f]", v.x, v.y, v.z);
}

inline void dump (const vec4f& v)
{
  printf ("[%+.2f %+.2f %+.2f %+.2f]", v.x, v.y, v.z, v.w);
}

inline void dump (const quatf& q)
{
/*  float pitch, yaw, roll;
  
  quat2EulerAngle (q, pitch, yaw, roll);
  
  pitch = fmod (rad2deg (pitch), 360.0f);
  yaw   = fmod (rad2deg (yaw), 360.0f);
  roll  = fmod (rad2deg (roll), 360.0f);

  printf ("[%+.1f %+.1f %+.1f]", pitch, yaw, roll);*/
  
  printf ("[%+.1f %+.1f %+.1f %+.1f]", prepare_float (q.w), prepare_float (q.x), prepare_float (q.y), prepare_float (q.z));
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

stl::basic_string<unsigned int> toutf32 (const wchar_t* string)
{
  if (sizeof (wchar_t) == 4)
    return stl::basic_string<unsigned int> ((const unsigned int*)string);

  stl::basic_string<unsigned int> result;
  
  result.fast_resize (xtl::xstrlen (string));

  const void* source           = string;
  size_t      source_size      = result.size () * sizeof (wchar_t);
  void*       destination      = &result [0];
  size_t      destination_size = result.size () * sizeof (unsigned int);

  convert_encoding (common::Encoding_UTF16LE, source, source_size, common::Encoding_UTF32LE, destination, destination_size);  
  
  return result;
}

#endif
