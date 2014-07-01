#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    ќписание реализации камеры
*/

struct Camera::Impl
{
  stl::string                name;                     //им€ камеры
  math::mat4f                world_tm;                 //матрица мировых преобразований камеры
  math::mat4f                proj_tm;                  //матрица проецировани€
  math::mat4f                view_tm;                  //матрица вида
  math::mat4f                view_proj_tm;             //матрица вид-проецирование
  bound_volumes::plane_listf frustum;                  //пирамида видимости
  size_t                     transaction_id;           //идентификатор изменениц параметров камеры
  size_t                     view_transaction_id;      //идентификатор изменений матрицы вида
  size_t                     proj_transaction_id;      //идентификатор изменений матрицы проецировани€
  size_t                     view_proj_transaction_id; //идентификатор изменений матрицы вид-проецирование
  bool                       need_update_view_tm;      //необходимо обновить матрицу вида
  bool                       need_update_view_proj_tm; //необходимо обновить матрицу проецировани€
  bool                       need_update_frustum;      //необходимо обновить пирамиду видимости

///  онструктор
  Impl ()
    : world_tm (1.0f)
    , proj_tm (1.0f)
    , view_tm (1.0f)
    , view_proj_tm (1.0f)
    , transaction_id (1)
    , view_transaction_id (1)
    , proj_transaction_id (1)
    , view_proj_transaction_id (1)
    , need_update_view_tm (false)
    , need_update_view_proj_tm (false)
    , need_update_frustum (true)
  {
    static const size_t FRUSTUM_PLANES_COUNT = 6;

    frustum.reserve (FRUSTUM_PLANES_COUNT);
  }
};

/*
     онструкторы / деструктор / присваивание
*/

Camera::Camera ()
  : impl (new Impl)
{
}

Camera::~Camera ()
{
}

/*
    »ндекс изменений
*/

size_t Camera::TransactionId () const
{
  return impl->transaction_id;
}

/*
    »м€ области вывода
*/

void Camera::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Camera::SetName", "name");

  impl->name = name;

  impl->transaction_id++;
}

const char* Camera::Name () const
{
  return impl->name.c_str ();
}

/*
    ћатрица вида
*/

void Camera::SetWorldMatrix (const math::mat4f& tm)
{
  impl->world_tm                 = tm;
  impl->need_update_view_tm      = true;
  impl->need_update_view_proj_tm = true;
  impl->need_update_frustum      = true;

  impl->view_transaction_id++;
  impl->view_proj_transaction_id++;
  impl->transaction_id++;
}

const math::mat4f& Camera::ViewMatrix () const
{
  if (!impl->need_update_view_tm)
    return impl->view_tm;

  impl->view_tm             = math::inverse (impl->world_tm);
  impl->need_update_view_tm = false;

  return impl->view_tm;
}

const math::mat4f& Camera::WorldMatrix () const
{
  return impl->world_tm;
}

size_t Camera::ViewMatrixTransactionId () const
{
  return impl->view_transaction_id;
}

/*
    ћатрица проецировани€
*/

void Camera::SetProjectionMatrix (const math::mat4f& tm)
{
  static float inv_ortx_tm_array [4][4] = {
    {-1, 0, 0, 0},
    { 0, 1, 0, 0},
    { 0, 0, 1, 0},
    { 0, 0, 0, 1},
  };

  static math::mat4f inv_ortx_tm (&inv_ortx_tm_array [0][0]);

  impl->proj_tm                  = inv_ortx_tm * tm;
  impl->need_update_view_proj_tm = true;
  impl->need_update_frustum      = true;

  impl->proj_transaction_id++;
  impl->view_proj_transaction_id++;
  impl->transaction_id++;
}

const math::mat4f& Camera::ProjectionMatrix () const
{
  return impl->proj_tm;
}

size_t Camera::ProjectionMatrixTransactionId () const
{
  return impl->proj_transaction_id;
}

/*
    ћатрица суперпозиции преобразований
*/

const math::mat4f& Camera::ViewProjectionMatrix () const
{
  if (!impl->need_update_view_proj_tm)
    return impl->view_proj_tm;

  impl->view_proj_tm             = impl->proj_tm * ViewMatrix ();
  impl->need_update_view_proj_tm = false;  

  return impl->view_proj_tm;
}

size_t Camera::ViewProjectionMatrixTransactionId () const
{
  return impl->view_proj_transaction_id;
}

/*
    ѕолучение пирамиды видимости
*/

const bound_volumes::plane_listf& Camera::Frustum () const
{
  if (!impl->need_update_frustum)
    return impl->frustum;

  impl->frustum.clear ();

  add_frustum (ViewProjectionMatrix (), impl->frustum);

  impl->need_update_frustum = false;

  return impl->frustum;
}

size_t Camera::FrustumTransactionId () const
{
  return impl->view_proj_transaction_id;
}
