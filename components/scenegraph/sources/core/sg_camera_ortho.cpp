#include <sg/camera.h>
#include <xtl/visitor.h>
#include <common/exception.h>

using namespace scene_graph;
using namespace math;
using namespace common;

const float EPS = 1e-6f;

/*
    ќписание реализации OrthoCamera
*/

struct OrthoCamera::Impl
{
  float left;   //лева€ граница камеры
  float right;  //права€ граница камеры
  float top;    //верхн€€ граница камеры
  float bottom; //нижн€€ граница камеры
  float z_near; //ближн€€ плоскость отсечени€
  float z_far;  //дальн€€ плоскость отсечени€
};

/*
     онструктор / деструктор
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
    —оздание камеры
*/

OrthoCamera* OrthoCamera::Create ()
{
  return new OrthoCamera;
}

/*
    ѕараметры
*/

void OrthoCamera::SetLeft (float left)
{
  impl->left = left;
}

float OrthoCamera::Left () const
{
  return impl->left;
}

void OrthoCamera::SetRight (float right)
{
  impl->right = right;
}

float OrthoCamera::Right () const
{
  return impl->right; 
}

void OrthoCamera::SetTop (float top)
{
  impl->top = top;
}

float OrthoCamera::Top () const
{
  return impl->top; 
}

void OrthoCamera::SetBottom (float bottom)
{
  impl->bottom = bottom;
}

float OrthoCamera::Bottom () const
{
  return impl->bottom; 
}

void OrthoCamera::SetZNear (float z_near)
{
  impl->z_near = z_near;
}

float OrthoCamera::ZNear () const
{
  return impl->z_near;
}

void OrthoCamera::SetZFar  (float z_far)
{
  impl->z_far = z_far;
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

  if (fabs (width)  < EPS) Raise<Exception> ("scene_graph::OrthoCamera::ComputeProjectionMatrix", "Zero camera width");
  if (fabs (height) < EPS) Raise<Exception> ("scene_graph::OrthoCamera::ComputeProjectionMatrix", "Zero camera height");
  if (fabs (depth)  < EPS) Raise<Exception> ("scene_graph::OrthoCamera::ComputeProjectionMatrix", "Zero camera depth");
  
  //выбрана матрица проецировани€, используема€ gluOrtho2D
  proj_matrix [0] = vec4f (2.0f / width, 0, 0, - (impl->right + impl->left) / width);
  proj_matrix [1] = vec4f (0, 2.0f / height, 0, - (impl->top + impl->bottom) / height);
  proj_matrix [2] = vec4f (0, 0, -2.0f / depth, - (impl->z_near + impl->z_far) / depth);
  proj_matrix [3] = vec4f (0, 0, 0, 1);
}

/*
    ƒинамическа€ диспетчеризаци€    
*/

void OrthoCamera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Camera::AcceptCore (visitor);
}
