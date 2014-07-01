#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

/*
    Описание реализации Camera
*/

struct Camera::Impl: public xtl::instance_counter<Camera>
{
  mat4f       proj_matrix;     //матрица проецирования
  plane_listf frustum;         //пирамида видимости
  bool        dirty_pm;        //при true матрица проецирования нуждается в перерасчёте
  bool        dirty_frustum;   //при true пирамида видимости нуждается в перерасчёте
};

/*
    Конструктор / деструктор
*/

Camera::Camera ()
  : impl (new Impl)
{
  impl->dirty_pm      = true;
  impl->dirty_frustum = true;

  static const size_t FRUSTUM_PLANES_COUNT = 6;

  impl->frustum.reserve (FRUSTUM_PLANES_COUNT);
}

Camera::~Camera ()
{
  delete impl;
}

/*
    Матрица проекции
*/

const mat4f& Camera::ProjectionMatrix () const
{
  if (impl->dirty_pm)
  {
    const_cast<Camera&> (*this).ComputeProjectionMatrix (impl->proj_matrix);
    
    impl->dirty_pm = false;
  }

  return impl->proj_matrix;
}

/*
    Получение пирамиды видимости
*/

const plane_listf& Camera::Frustum () const
{
  if (impl->dirty_frustum)
  {
    impl->frustum.clear ();

    add_frustum (ProjectionMatrix () * math::inverse (WorldTM ()), impl->frustum);

    impl->dirty_frustum = false;
  }
  
  return impl->frustum;
}

/*
    Сигнал обновления матрицы
*/

void Camera::UpdateCameraNotify ()
{
  impl->dirty_pm      = true;
  impl->dirty_frustum = true;

  UpdateNotify ();
}

void Camera::AfterUpdateWorldTransformEvent ()
{
  impl->dirty_frustum = true;
}

/*
    Метод, вызываемый при посещении объекта
*/

void Camera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
