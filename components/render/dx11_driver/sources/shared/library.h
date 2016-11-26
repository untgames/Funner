#ifndef RENDER_DX11_DRIVER_LIBRARY_HEADER
#define RENDER_DX11_DRIVER_LIBRARY_HEADER

#include <D3DX11async.h>

#include <shared/common.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневый драйвер
///////////////////////////////////////////////////////////////////////////////////////////////////
class Library
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Library  ();
    Library  (const Library&);
    ~Library ();

    Library& operator = (const Library&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    HRESULT D3D11CreateDevice (
      IDXGIAdapter*            pAdapter,
      D3D_DRIVER_TYPE          DriverType,
      HMODULE                  Software,
      UINT                     Flags,
      const D3D_FEATURE_LEVEL* pFeatureLevels,
      UINT                     FeatureLevels,
      UINT                     SDKVersion,
      ID3D11Device**           ppDevice,
      D3D_FEATURE_LEVEL*       pFeatureLevel,
      ID3D11DeviceContext**    ppImmediateContext);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание DXGI фабрики
///////////////////////////////////////////////////////////////////////////////////////////////////
    HRESULT CreateDXGIFactory (REFIID riid, void **ppFactory);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Компиляция шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
    HRESULT D3DX11CompileFromMemory (
      LPCSTR                    pSrcData,
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
      HRESULT*                  pHResult);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о шейдере
///////////////////////////////////////////////////////////////////////////////////////////////////
      HRESULT D3DReflect (LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector); 

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
