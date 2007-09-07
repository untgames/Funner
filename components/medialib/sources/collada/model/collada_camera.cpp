#include <media/collada/scene.h>

using namespace media::collada;
using namespace common;
using namespace math;

/*
    Описание реализации камеры
*/

struct Camera::Impl
{
  CameraType type;                     //тип камеры
  float      params [CameraParam_Num]; //параметры камеры
  
  Impl () : type (CameraType_Perspective)
  {
    for (size_t i=0; i<LightParam_Num; i++)
      params [i] = 0.0f;
  }
};

/*
    Конструктор / деструктор
*/

Camera::Camera (ModelImpl* owner, const char* id)
  : Entity (owner, id), impl (new Impl)
  {}

Camera::~Camera ()
{
  delete impl;
}
  
/*
    Тип источника
*/

CameraType Camera::Type () const
{
  return impl->type;
}

void Camera::SetType (CameraType type)
{
  switch (type)
  {
    case CameraType_Perspective:
    case CameraType_Orthographic:
      break;
    default:
      RaiseInvalidArgument ("media::collada::Camera::SetType", "type", type);
      break;
  }
  
  impl->type = type;
}

/*
    Параметры камеры
*/

void Camera::SetParam (CameraParam param, float value)
{
  if (param < 0 || param >= CameraParam_Num)
    RaiseInvalidArgument ("media::collada::Camera::SetParam", "param", param);
    
  impl->params [param] = value;
}

float Camera::Param (CameraParam param) const
{
  if (param < 0 || param >= CameraParam_Num)
    RaiseInvalidArgument ("media::collada::Camera::Param", "param", param);
    
  return impl->params [param];
}
