#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    DeviceManager
*/

struct DeviceManager::Impl: public xtl::reference_counter
{
  DxDevicePtr  device;
  DxContextPtr context;

  Impl (const DxDevicePtr& in_device) : device (in_device) 
  {
    static const char* METHOD_NAME = "render::low_level::dx11::DeviceManager::Impl::Impl";

    if (!device)
      throw xtl::make_null_argument_exception (METHOD_NAME, "device");

    ID3D11DeviceContext* dx_context = 0;

    device->GetImmediateContext (&dx_context);

    if (!dx_context)
      throw xtl::format_operation_exception (METHOD_NAME, "ID3D11Device::GetImmediateContext failed");

    context = dx_context;
  }
};

DeviceManager::DeviceManager (const DxDevicePtr& device)
  : impl (new Impl (device))
{
}

DeviceManager::DeviceManager (const DeviceManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

DeviceManager::~DeviceManager ()
{
  release (impl);
}

DeviceManager& DeviceManager::operator = (const DeviceManager& manager)
{
  DeviceManager (manager).Swap (*this);

  return *this;
}

ID3D11Device& DeviceManager::GetDevice () const
{
  return *impl->device;
}

ID3D11DeviceContext& DeviceManager::GetImmediateContext () const
{
  return *impl->context;
}

void DeviceManager::Swap (DeviceManager& manager)
{
  stl::swap (impl, manager.impl);
}

/*
    DeviceObject
*/

DeviceObject::DeviceObject (const DeviceManager& in_manager)
  : manager (in_manager)
{
}

void DeviceObject::GetFamily (ObjectFamily& family, void*& root)
{
  family.id     = FAMILY_ID;
  family.sub_id = manager.GetFamilySubId ();
  root          = static_cast<Object*> (this);
}

ID3D11Device& DeviceObject::GetDevice () const
{
  return manager.GetDevice ();
}
