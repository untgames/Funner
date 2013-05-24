#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

namespace
{

/*
    Константы
*/

const char* DXGI_LIBRARY_NAME  = "dxgi.dll";
const char* D3D11_LIBRARY_NAME = "d3d11.dll";

}

/*
    Описание реализации библиотеки
*/

typedef HRESULT (WINAPI *PFN_CREATE_DXGI_FACTORY) (REFIID riid, void **ppFactory);

struct Library::Impl: public xtl::reference_counter
{
  stl::auto_ptr<syslib::DynamicLibrary> dxgi_library;           //библиотека DXGI
  stl::auto_ptr<syslib::DynamicLibrary> d3d11_library;          //библиотека D3D11
  PFN_D3D11_CREATE_DEVICE               create_device_fn;       //функция создания устройства
  PFN_CREATE_DXGI_FACTORY               create_dxgi_factory_fn; //создание фабрики DXGI

  static Impl*         global_impl;
  static syslib::Mutex global_impl_mutex;

/// Конструктор
  Impl ()
    : create_device_fn ()
    , create_dxgi_factory_fn ()
  {
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      syslib::Lock lock (global_impl_mutex);

      if (global_impl == this)
        global_impl = 0;
    }
    catch (...)
    {
    }
  }

/// Получение реализации
  static Impl* GetImpl ()
  {
    syslib::Lock lock (global_impl_mutex);

    if (global_impl)
    {
      addref (global_impl);
      return global_impl;
    }

    global_impl = new Impl;

    return global_impl;
  }

/// Загрузка библиотек
  void LoadD3D11Library ()
  {
    if (d3d11_library)
      return;

    d3d11_library.reset (new syslib::DynamicLibrary (D3D11_LIBRARY_NAME));
  }

  void LoadDxgiLibrary ()
  {
    if (dxgi_library)
      return;

    dxgi_library.reset (new syslib::DynamicLibrary (DXGI_LIBRARY_NAME));
  }
};

Library::Impl* Library::Impl::global_impl = 0;
syslib::Mutex  Library::Impl::global_impl_mutex;

/*
    Конструкторы / деструктор / присваивание
*/

Library::Library ()
{
  impl = Impl::GetImpl ();
}

Library::Library (const Library& library)
  : impl (library.impl)
{
  addref (impl);
}

Library::~Library ()
{
  release (impl);
}

Library& Library::operator = (const Library& library)
{
  impl = library.impl;

  addref (impl);

  return *this;
}

/*
    Создание устройства
*/

HRESULT Library::CreateDevice (
 IDXGIAdapter*            pAdapter,
 D3D_DRIVER_TYPE          DriverType,
 HMODULE                  Software,
 UINT                     Flags,
 const D3D_FEATURE_LEVEL* pFeatureLevels,
 UINT                     FeatureLevels,
 UINT                     SDKVersion,
 ID3D11Device**           ppDevice,
 D3D_FEATURE_LEVEL*       pFeatureLevel,
 ID3D11DeviceContext**    ppImmediateContext)
{
  try
  {
    if (!impl->create_device_fn)
    {
      impl->LoadD3D11Library ();

      static const char* ENTRY_NAME = "D3D11CreateDevice";
      
      impl->create_device_fn = (PFN_D3D11_CREATE_DEVICE)impl->d3d11_library->GetSymbol (ENTRY_NAME);

      if (!impl->create_device_fn)
        throw xtl::format_operation_exception ("", "Symbol %s not found in %s", ENTRY_NAME, D3D11_LIBRARY_NAME);
    }

    return impl->create_device_fn (pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Library::CreateDevice");
    throw;
  }
}

/*
    Создание DXGI фабрики
*/

HRESULT Library::CreateDXGIFactory (REFIID riid, void** ppFactory)
{
  try
  {
    if (!impl->create_dxgi_factory_fn)
    {
      impl->LoadDxgiLibrary ();

      static const char* ENTRY_NAME = "CreateDXGIFactory";

      impl->create_dxgi_factory_fn = (PFN_CREATE_DXGI_FACTORY)impl->dxgi_library->GetSymbol (ENTRY_NAME);

      if (!impl->create_dxgi_factory_fn)
        throw xtl::format_operation_exception ("", "Symbol %s not found in %s", ENTRY_NAME, DXGI_LIBRARY_NAME);
    }

    return impl->create_dxgi_factory_fn (riid, ppFactory);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Library::CreateDXGIFactory");
    throw;
  }
}
