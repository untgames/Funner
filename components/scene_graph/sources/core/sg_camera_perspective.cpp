#include <sg/camera.h>
#include <xtl/visitor.h>
#include <common/exception.h>
#include <bv/axis_aligned_box.h>

#include <stl/algorithm>

using namespace scene_graph;
using namespace math;
using namespace common;
using namespace bound_volumes;

const float EPS = 1e-6f;

/*
    Описание реализации PerspectiveCamera
*/

struct PerspectiveCamera::Impl
{
  float fov_x;  //угол обзора по горизонтали
  float fov_y;  //угол обзора по вертикали
  float z_near; //ближняя плоскость отсечения
  float z_far;  //дальняя плоскость отсечения
};

/*
    Конструктор / деструктор
*/

PerspectiveCamera::PerspectiveCamera ()
  : impl (new Impl)
{
  impl->fov_x  = 1;
  impl->fov_y  = 1;
  impl->z_near = 0;
  impl->z_far  = 1;
}

PerspectiveCamera::~PerspectiveCamera ()
{
  delete impl;
}

/*
    Создание камеры
*/

PerspectiveCamera::Pointer PerspectiveCamera::Create ()
{
  return Pointer (new PerspectiveCamera, false);
}

/*
    Параметры
*/

void PerspectiveCamera::SetFovX (float fov_x)
{
  impl->fov_x = fov_x;
  UpdateBoundsNotify ();
}

float PerspectiveCamera::FovX () const
{
  return impl->fov_x;
}

void PerspectiveCamera::SetFovY (float fov_y)
{
  impl->fov_y = fov_y;
  UpdateBoundsNotify ();
}

float PerspectiveCamera::FovY () const
{
  return impl->fov_y; 
}

void PerspectiveCamera::SetZNear (float z_near)
{
  impl->z_near = z_near;
  UpdateBoundsNotify ();
}

float PerspectiveCamera::ZNear () const
{
  return impl->z_near;
}

void PerspectiveCamera::SetZFar  (float z_far)
{
  impl->z_far = z_far;
  UpdateBoundsNotify ();
}

float PerspectiveCamera::ZFar () const
{
  return impl->z_far;
}

void PerspectiveCamera::ComputeProjectionMatrix (math::mat4f& proj_matrix)
{
  float width  = tan (deg2rad (impl->fov_x)) * impl->z_near, 
        height = tan (deg2rad (impl->fov_y)) * impl->z_near,
        depth  = impl->z_far - impl->z_near;

  if (fabs (width)  < EPS) raise <Exception> ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera width");
  if (fabs (height) < EPS) raise <Exception> ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera height");
  if (fabs (depth) < EPS)  raise <Exception> ("scene_graph::PerspectiveCamera::ComputeProjectionMatrix", "Zero camera depth");

  proj_matrix [0] = vec4f (2.0f * impl->z_near / width, 0, 0, 0);
  proj_matrix [1] = vec4f (0, 2.0f * impl->z_near / height, 0, 0);
  proj_matrix [2] = vec4f (0, 0, -(impl->z_far + impl->z_near) / depth, -2.0f * impl->z_near * impl->z_far / depth);
  proj_matrix [3] = vec4f (0, 0, -1, 0);
}

/*
   Рассчёт ограничивающего объёма
*/

void PerspectiveCamera::UpdateBoundsCore ()
{
  float half_width  = tan (deg2rad (impl->fov_x / 2)) * stl::max (fabs (impl->z_near), fabs (impl->z_far)), 
        half_height = tan (deg2rad (impl->fov_y / 2)) * stl::max (fabs (impl->z_near), fabs (impl->z_far));

  SetBoundBox (axis_aligned_box <float> (-half_width, -half_height, -impl->z_far,
                                          half_width,  half_height, -impl->z_near));
}

/*
    Динамическая диспетчеризация    
*/

void PerspectiveCamera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Camera::AcceptCore (visitor);
}
