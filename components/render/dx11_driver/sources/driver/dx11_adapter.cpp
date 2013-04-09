#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

Adapter::Adapter (const DxFactoryPtr& factory, const char* name, const char* dll_path, const char*)
{
  try
  {
    if (!factory)
      throw xtl::make_null_argument_exception ("", "factory");

    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");

    HMODULE module = LoadLibrary (dll_path);

    if (!module)
      throw xtl::format_operation_exception ("", "Can't load module '%s'", dll_path);

    try
    {
      IDXGIAdapter* in_adapter = 0;

      check_errors ("IDXGIFactory::CreateSoftwareAdapter", factory->CreateSoftwareAdapter (module, &in_adapter));

      if (!in_adapter)
        throw xtl::format_operation_exception ("", "IDXGIFactory::CreateSoftwareAdapter failed");

      adapter = in_adapter;

      Init ();

      this->name = name;
      this->path = dll_path;
    }
    catch (...)
    {
      FreeLibrary (module);
      throw;
    }

    FreeLibrary (module);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Adapter::Adapter(const char*,const char*,const char*)");
    throw;
  }
}

Adapter::Adapter (const DxAdapterPtr& in_adapter)
{
  try
  {
    if (!in_adapter)
      throw xtl::make_null_argument_exception ("", "adapter");

    adapter = in_adapter;

    Init ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Adapter::Adapter(IDXGIAdapter*)");
    throw;
  }
}

void Adapter::Init ()
{
  DXGI_ADAPTER_DESC desc;

  check_errors ("IDXGIAdapter::GetDesc", adapter->GetDesc (&desc));
  
  name = common::tostring (desc.Description);
  path = "unknown";

  properties.AddProperty ("Description", name.c_str ());
  properties.AddProperty ("VendorId", common::format ("%08x", desc.VendorId).c_str ());
  properties.AddProperty ("DeviceId", common::format ("%08x", desc.DeviceId).c_str ());
  properties.AddProperty ("SubSysId", common::format ("%08x", desc.SubSysId).c_str ());
  properties.AddProperty ("Revision", common::format ("%08x", desc.Revision).c_str ());
  properties.AddProperty ("DedicatedVideoMemory", common::format ("%u", desc.DedicatedVideoMemory).c_str ());
  properties.AddProperty ("DedicatedSystemMemory", common::format ("%u", desc.DedicatedSystemMemory).c_str ());
  properties.AddProperty ("SharedSystemMemory", common::format ("%u", desc.SharedSystemMemory).c_str ());
  properties.AddProperty ("AdapterLuid", common::format ("%08x", desc.AdapterLuid).c_str ());
}

Adapter::~Adapter ()
{
}

/*
    Имя адаптера / путь к модулю / описание
*/

const char* Adapter::GetName ()
{
  return name.c_str ();
}

const char* Adapter::GetPath ()
{
  return path.c_str ();
}

const char* Adapter::GetDescription ()
{
  return "render::low_level::dx11::Adapter";
}

/*
    Перечисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IOutput* Adapter::GetOutput (size_t index)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Список свойств адаптера
*/

IPropertyList* Adapter::GetProperties ()
{
  return &properties;
}
