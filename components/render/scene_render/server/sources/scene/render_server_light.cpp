#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации отображаемой модели
*/

namespace
{

const float DEFAULT_LIGHT_RANGE  = 1e9f;   //расстояние действия источника света по умолчанию
const float DEFAULT_LIGHT_RADIUS = 1e9f;   //радиус действия источника света по умолчанию
const float ZNEAR_FACTOR         = 1e-06f; //коэффициент перехода от znear к zfar

typedef xtl::shared_ptr<Camera> CameraPtr;
typedef stl::vector<CameraPtr>  CameraArray;

}

struct Light::Impl
{
  interchange::NodeType node_type;           //тип источника
  LightParams           params;              //параметры источника
  CameraArray           cameras;             //камеры "из источника"
  bool                  need_update_cameras; //камеры нужно обновить

/// Конструктор
  Impl (interchange::NodeType in_node_type)
    : node_type (in_node_type)
    , need_update_cameras (true)
  {
    params.color       = math::vec3f (1.0f);
    params.intensity   = 1.0f;
    params.attenuation = 0.0f;
    params.range       = DEFAULT_LIGHT_RANGE;
    params.angle       = math::anglef ();
    params.exponent    = 0.0f;
    params.radius      = DEFAULT_LIGHT_RADIUS;

    switch (node_type)
    {
      case interchange::NodeType_PointLight:
      case interchange::NodeType_SpotLight:
      case interchange::NodeType_DirectLight:
        break;
      default:
        throw xtl::make_argument_exception ("render::scene::server::Light::Light", "node_type", node_type);
    }
  }

///Обновление камер
  void UpdateCameras (const Light& owner)
  {
    if (!need_update_cameras)
      return;

    try
    {
      const math::mat4f& world_tm = owner.WorldMatrix ();

      switch (node_type)
      {
        case interchange::NodeType_PointLight:
        {
          SetupCameras (6);

          float znear = ZNEAR_FACTOR * params.range;

          math::mat4f base_proj_tm = ComputePerspectiveMatrix (math::radian (math::degree (90.0f)), znear, params.range);

          math::vec3f points [6][2] = {
            {math::vec3f (znear, 0, 0),  math::vec3f (0, 1.0f, 0)},
            {math::vec3f (-znear, 0, 0), math::vec3f (0, 1.0f, 0)},

            {math::vec3f (0, znear, 0),  math::vec3f (0, 0, -1.0f)},
            {math::vec3f (0, -znear, 0), math::vec3f (0, 0, 1.0f)},

            {math::vec3f (0, 0, znear),  math::vec3f (0, 1.0f, 0)},
            {math::vec3f (0, 0, -znear), math::vec3f (0, 1.0f, 0)},
          };

          for (size_t i=0; i<6; i++)
          {
            math::mat4f view_tm = lookat (math::vec3f (0), points [i][0], points [i][1]);

            cameras [i]->SetProjectionMatrix (base_proj_tm);
            cameras [i]->SetWorldMatrix      (view_tm * world_tm);
          }

          break;
        }
        case interchange::NodeType_SpotLight:
        {
          SetupCameras (1);

          cameras [0]->SetProjectionMatrix (ComputePerspectiveMatrix (math::radian (params.angle), ZNEAR_FACTOR * params.range, params.range));
          cameras [0]->SetWorldMatrix      (world_tm);

          break;
        }
        case interchange::NodeType_DirectLight:
        {
          SetupCameras (1);

          cameras [0]->SetProjectionMatrix (ComputeOrthoMatrix (-params.radius, params.radius, -params.radius, params.radius, 0, params.range));
          cameras [0]->SetWorldMatrix      (world_tm);

          break;
        }
        default:
          throw xtl::format_operation_exception ("", "Invalid camera type %d", node_type);
      }

      need_update_cameras = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::Light::Impl::UpdateCameras");
      throw;
    }
  }

  void SetupCameras (size_t count)
  {
    cameras.resize (count);

    for (CameraArray::iterator iter=cameras.begin (), end=cameras.end (); iter!=end; ++iter)
    {
      CameraPtr& camera = *iter;

      if (!camera)
        camera = CameraPtr (new server::Camera);
    }
  }

  static math::mat4f ComputeOrthoMatrix (float left, float right, float bottom, float top, float znear, float zfar)
  {
    math::mat4f proj_matrix;

    float width  = right - left,
          height = top - bottom,
          depth  = zfar - znear;

    proj_matrix [0] = math::vec4f (2.0f / width, 0, 0, -(right + left) / width);
    proj_matrix [1] = math::vec4f (0, 2.0f / height, 0, - (top + bottom) / height);
    proj_matrix [2] = math::vec4f (0, 0, 2.0f / depth, -(znear + zfar) / depth);
    proj_matrix [3] = math::vec4f (0, 0, 0, 1);

    return proj_matrix;
  }

  static math::mat4f ComputePerspectiveMatrix (float fov, float znear, float zfar)
  {
    math::mat4f proj_matrix;

    float width  = 2.f * tan (fov * 0.5f) * znear,
          height = 2.f * tan (fov * 0.5f) * znear,
          depth  = zfar - znear;

    proj_matrix [0] = math::vec4f (-2.0f * znear / width, 0, 0, 0);
    proj_matrix [1] = math::vec4f (0, 2.0f * znear / height, 0, 0);
    proj_matrix [2] = math::vec4f (0, 0, (zfar + znear) / depth, -2.0f * znear * zfar / depth);
    proj_matrix [3] = math::vec4f (0, 0, 1, 0);

    return proj_matrix;
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Light::Light (interchange::NodeType node_type)
  : impl (new Impl (node_type))
{
}

Light::~Light ()
{
}

/*
    Тип источника
*/

interchange::NodeType Light::Type () const
{
  return impl->node_type;
}

/*
    Параметры источника
*/

void Light::SetParams (const LightParams& params)
{
  impl->params              = params;
  impl->need_update_cameras = true;
}

const LightParams& Light::Params () const
{
  return impl->params;
}

/*
    Перечисление камер из источника
*/

size_t Light::CamerasCount () const
{
  switch (impl->node_type)
  {
    case interchange::NodeType_PointLight:  return 6;
    case interchange::NodeType_SpotLight:  
    case interchange::NodeType_DirectLight: return 1;
    default:                                return 0;
  }
}

const Camera& Light::Camera (size_t index) const
{
  impl->UpdateCameras (*this);

  if (index >= impl->cameras.size ())
    throw xtl::make_range_exception ("render::scene::server::Light::Camera", "index", index, impl->cameras.size ());

  return *impl->cameras [index];
}

/*
    Оповещение об обновлении мировой матрицы
*/

void Light::OnWorldMatrixUpdated ()
{
  Entity::OnWorldMatrixUpdated ();

  impl->need_update_cameras = true;
}

/*
    Обход
*/

void Light::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
