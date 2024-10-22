#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace common;
using namespace bound_volumes;

const float EPS = 1e-6f;

/*
    Описание реализации OrthoCamera
*/

struct OrthoCamera::Impl: public xtl::instance_counter<OrthoCamera>
{
  float left;   //левая граница камеры
  float right;  //правая граница камеры
  float top;    //верхняя граница камеры
  float bottom; //нижняя граница камеры
  float z_near; //ближняя плоскость отсечения
  float z_far;  //дальняя плоскость отсечения
};

/*
    Конструктор / деструктор
*/

OrthoCamera::OrthoCamera ()
  : impl (new Impl)
{
  impl->left   = 0;
  impl->right  = 1;
  impl->top    = 1;
  impl->bottom = 0;
  impl->z_near = 0;
  impl->z_far  = 1;
}

OrthoCamera::~OrthoCamera ()
{
  delete impl;
}

/*
    Создание камеры
*/

OrthoCamera::Pointer OrthoCamera::Create ()
{
  return Pointer (new OrthoCamera, false);
}

/*
    Параметры
*/

void OrthoCamera::SetLeft (float left)
{
  impl->left = left;
  UpdateBoundsNotify ();
  UpdateCameraNotify ();  
}

float OrthoCamera::Left () const
{
  return impl->left;
}

void OrthoCamera::SetRight (float right)
{
  impl->right = right;
  UpdateBoundsNotify ();
  UpdateCameraNotify ();
}

float OrthoCamera::Right () const
{
  return impl->right; 
}

void OrthoCamera::SetTop (float top)
{
  impl->top = top;
  UpdateBoundsNotify ();
  UpdateCameraNotify ();
}

float OrthoCamera::Top () const
{
  return impl->top; 
}

void OrthoCamera::SetBottom (float bottom)
{
  impl->bottom = bottom;
  UpdateBoundsNotify ();
  UpdateCameraNotify ();
}

float OrthoCamera::Bottom () const
{
  return impl->bottom; 
}

void OrthoCamera::SetZNear (float z_near)
{
  impl->z_near = z_near;
  UpdateBoundsNotify ();
  UpdateCameraNotify ();
}

float OrthoCamera::ZNear () const
{
  return impl->z_near;
}

void OrthoCamera::SetZFar  (float z_far)
{
  impl->z_far = z_far;
  UpdateBoundsNotify ();
  UpdateCameraNotify ();
}

float OrthoCamera::ZFar () const
{
  return impl->z_far;
}

void OrthoCamera::ComputeProjectionMatrix (math::mat4f& proj_matrix)
{
  float width  = impl->right - impl->left,
        height = impl->top - impl->bottom,
        depth  = impl->z_far - impl->z_near;

  if (fabs (width)  < EPS) throw xtl::format_operation_exception ("scene_graph::OrthoCamera::ComputeProjectionMatrix", "Zero camera width");
  if (fabs (height) < EPS) throw xtl::format_operation_exception ("scene_graph::OrthoCamera::ComputeProjectionMatrix", "Zero camera height");
  if (fabs (depth)  < EPS) throw xtl::format_operation_exception ("scene_graph::OrthoCamera::ComputeProjectionMatrix", "Zero camera depth");
  
  proj_matrix [0] = vec4f (2.0f / width, 0, 0, -(impl->right + impl->left) / width);
  proj_matrix [1] = vec4f (0, 2.0f / height, 0, - (impl->top + impl->bottom) / height);
  proj_matrix [2] = vec4f (0, 0, 2.0f / depth, -(impl->z_near + impl->z_far) / depth);
  proj_matrix [3] = vec4f (0, 0, 0, 1);
}

/*
   Рассчёт ограничивающего объёма
*/

void OrthoCamera::UpdateBoundsCore ()
{
  SetBoundBox (axis_aligned_box <float> (stl::min (impl->left, impl->right), stl::min (impl->bottom, impl->top), -impl->z_far,
                                         stl::max (impl->left, impl->right), stl::max (impl->bottom, impl->top), impl->z_near));
}

/*
    Динамическая диспетчеризация    
*/

void OrthoCamera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Camera::AcceptCore (visitor);
}

/*
    Связывание свойств
*/

void OrthoCamera::BindProperties (common::PropertyBindingMap& bindings)
{
  Camera::BindProperties (bindings);

  bindings.AddProperty ("Left", xtl::bind (&OrthoCamera::Left, this), xtl::bind (&OrthoCamera::SetLeft, this, _1));
  bindings.AddProperty ("Right", xtl::bind (&OrthoCamera::Right, this), xtl::bind (&OrthoCamera::SetRight, this, _1));
  bindings.AddProperty ("Top", xtl::bind (&OrthoCamera::Top, this), xtl::bind (&OrthoCamera::SetTop, this, _1));
  bindings.AddProperty ("Bottom", xtl::bind (&OrthoCamera::Bottom, this), xtl::bind (&OrthoCamera::SetBottom, this, _1));
  bindings.AddProperty ("ZNear", xtl::bind (&OrthoCamera::ZNear, this), xtl::bind (&OrthoCamera::SetZNear, this, _1));
  bindings.AddProperty ("ZFar", xtl::bind (&OrthoCamera::ZFar, this), xtl::bind (&OrthoCamera::SetZFar, this, _1));
}
