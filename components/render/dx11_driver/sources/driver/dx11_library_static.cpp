#ifdef WINRT

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструкторы / деструктор / присваивание
*/

Library::Library ()
{
  impl = 0;
}

Library::Library (const Library&)
  : impl (0)
{
}

Library::~Library ()
{
}

Library& Library::operator = (const Library&)
{
  return *this;
}

/*
    Создание устройства
*/

HRESULT Library::D3D11CreateDevice (
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
  return ::D3D11CreateDevice (pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
}

/*
    Создание DXGI фабрики
*/

HRESULT Library::CreateDXGIFactory (REFIID riid, void** ppFactory)
{
  return ::CreateDXGIFactory (riid, ppFactory);
}

/*
    Компиляция шейдера
*/

HRESULT Library::D3DX11CompileFromMemory 
 (LPCSTR                    pSrcData,
  SIZE_T                    SrcDataLen,
  LPCSTR                    pFileName,
  const D3D10_SHADER_MACRO* pDefines,
  LPD3D10INCLUDE            pInclude,
  LPCSTR                    pFunctionName,
  LPCSTR                    pProfile,
  UINT                      Flags1,
  UINT                      Flags2,
  ID3DX11ThreadPump*        pPump,
  ID3D10Blob**              ppShader,
  ID3D10Blob**              ppErrorMsgs,
  HRESULT*                  pHResult)
{
#ifdef WINRT
  throw xtl::format_not_supported_exception ("render::low_level::dx11::Library::D3DX11CompileFromMemory", "D3DX11CompileFromMemory is not supported");
#else
  return ::D3DX11CompileFromMemory (pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags1, Flags2, pPump, ppShader, ppErrorMsgs, pHResult);
#endif
}

/*
    Получение информации о шейдере  
*/

HRESULT Library::D3DReflect (LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector)
{
#ifdef WINRT
  throw xtl::format_not_supported_exception ("render::low_level::dx11::Library::D3DReflect", "D3DReflect is not supported");
#else
  return ::D3DReflect (pSrcData, SrcDataSize, pInterface, ppReflector);
#endif
}

#endif
