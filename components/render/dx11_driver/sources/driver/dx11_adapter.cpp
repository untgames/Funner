#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Константы
*/

const size_t RESERVE_OUTPUTS_SIZE = 16;

/*
    Конструктор / деструктор
*/

Adapter::Adapter (const DxFactoryPtr& factory, const char* name, const char* dll_path, const char*)
  : module ()
{
  try
  {
    if (!factory)
      throw xtl::make_null_argument_exception ("", "factory");

    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!dll_path)
      throw xtl::make_null_argument_exception ("", "dll_path");

    module = LoadLibrary (dll_path);

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
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Adapter::Adapter(const char*,const char*,const char*)");
    throw;
  }
}

Adapter::Adapter (const DxAdapterPtr& in_adapter)
  : module ()
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
    //получение параметров адаптера

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

    //перечисление устройств вывода

  outputs.reserve (RESERVE_OUTPUTS_SIZE);

  IDXGIOutput* dx_output = 0;

  for (UINT i=0; adapter->EnumOutputs (i, &dx_output) != DXGI_ERROR_NOT_FOUND; i++)
  {
    OutputPtr output (new Output (dx_output), false);

    outputs.push_back (output);
  }  
}

Adapter::~Adapter ()
{
  if (module)
    FreeLibrary (module);
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

HMODULE Adapter::GetModule ()
{
  return module;
}

/*
    Ссылка на адаптер DX11
*/

IDXGIAdapter& Adapter::DxAdapter ()
{
  return *adapter;
}

/*
    Перечисление доступных устройств вывода
*/

size_t Adapter::GetOutputsCount ()
{
  return outputs.size ();
}

IOutput* Adapter::GetOutput (size_t index)
{
  if (index >= outputs.size ())
    throw xtl::make_range_exception ("render::low_level::dx11::Adapter::GetOutput", "index", index, outputs.size ());

  return &*outputs [index];
}

Output* Adapter::FindOutput (IDXGIOutput* output)
{
  for (OutputArray::iterator iter=outputs.begin (), end=outputs.end (); iter!=end; ++iter)
    if ((*iter)->GetOutput () == output)
      return &**iter;

  return 0;
}

/*
    Список свойств адаптера
*/

IPropertyList* Adapter::GetProperties ()
{
  return &properties;
}
