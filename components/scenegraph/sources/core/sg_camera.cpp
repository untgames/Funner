#include <sg/camera.h>
#include <xtl/visitor.h>

using namespace scene_graph;
using namespace math;

/*
    Описание реализации Camera
*/

struct Camera::Impl
{
  mat4f proj_matrix;  //матрица проецирования
  int   dirty_pm;     //при true матрица проецирования нуждается в перерасчёте
};

/*
    Конструктор / деструктор
*/

Camera::Camera ()
  : impl (new Impl)
{
  impl->proj_matrix = 1;
  impl->dirty_pm    = 1;
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
    impl->dirty_pm = 0;
  }
  return impl->proj_matrix;
}

/*
    Сигнал обновления матрицы
*/

void Camera::UpdateNotify ()
{
  impl->dirty_pm = 1;
}

/*
    Метод, вызываемый при посещении объекта
*/

void Camera::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
