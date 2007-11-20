#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор
*/

DeviceObject::DeviceObject (Device* in_device)
  : device (in_device)
  { }

/*
    Указатель на устройство отрисовки, которому принадлежит ресурс
*/

IDevice* DeviceObject::GetDevice ()
{
  common::RaiseNotImplemented ("render::low_level::opengl::DeviceObject::GetDevice");
  return 0;
}

/*
    Проверка совместимости объеков
*/

bool DeviceObject::IsCompatible (IObject* object)
{
  return IsCompatible (dynamic_cast<DeviceObject*> (object));
}

bool DeviceObject::IsCompatible (DeviceObject* object)
{
  return object && device == object->device;
}
