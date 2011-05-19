//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
// This is a prerelease header file and subject to change.
//
// d3dm.h
// Direct3D Mobile Interface Header File

#pragma once

#include <windows.h>

// This identifier is passed to Direct3DMobileCreate in order to ensure that an
// application was built against the correct header files. This number is
// incremented whenever a header (or other) change would require applications
// to be rebuilt. If the version doesn't match, Direct3DMobileCreate will fail.
// (The number itself has no meaning.)

#define D3DM_SDK_VERSION 120

// Interface IID's

// IID_IDirect3DMobile
// {0E1F27DB-2F90-4537-945E-ACE70B49B2D3}
DEFINE_GUID(IID_IDirect3DMobile, 
0xe1f27db, 0x2f90, 0x4537, 0x94, 0x5e, 0xac, 0xe7, 0xb, 0x49, 0xb2, 0xd3);

// IID_IDirect3DMobileDevice
// {7A4475F8-D4AF-4fd6-95D7-8C8526F37C66}
DEFINE_GUID(IID_IDirect3DMobileDevice, 
0x7a4475f8, 0xd4af, 0x4fd6, 0x95, 0xd7, 0x8c, 0x85, 0x26, 0xf3, 0x7c, 0x66);

// IID_IDirect3DMobileResource
// {1F424147-B267-48db-A0A8-1240D91BB494}
DEFINE_GUID(IID_IDirect3DMobileResource, 
0x1f424147, 0xb267, 0x48db, 0xa0, 0xa8, 0x12, 0x40, 0xd9, 0x1b, 0xb4, 0x94);

// IID_IDirect3DMobileBaseTexture
// {5CE7AA9B-BE1F-422f-B255-BA3B3645C227}
DEFINE_GUID(IID_IDirect3DMobileBaseTexture, 
0x5ce7aa9b, 0xbe1f, 0x422f, 0xb2, 0x55, 0xba, 0x3b, 0x36, 0x45, 0xc2, 0x27);

// IID_IDirect3DMobileTexture
// {E11FC2DD-4E8D-476b-AC13-942F4CDC0712}
DEFINE_GUID(IID_IDirect3DMobileTexture, 
0xe11fc2dd, 0x4e8d, 0x476b, 0xac, 0x13, 0x94, 0x2f, 0x4c, 0xdc, 0x7, 0x12);

// IID_IDirect3DMobileVertexBuffer
// {37167590-D11D-4cf1-9D79-2457961D48B3}
DEFINE_GUID(IID_IDirect3DMobileVertexBuffer, 
0x37167590, 0xd11d, 0x4cf1, 0x9d, 0x79, 0x24, 0x57, 0x96, 0x1d, 0x48, 0xb3);

// IID_IDirect3DMobileIndexBuffer
// {1930F2B3-1522-4dd0-8E45-C6D3F397C9F2}
DEFINE_GUID(IID_IDirect3DMobileIndexBuffer, 
0x1930f2b3, 0x1522, 0x4dd0, 0x8e, 0x45, 0xc6, 0xd3, 0xf3, 0x97, 0xc9, 0xf2);

// IID_IDirect3DMobileSurface
// {496A8F01-1842-4ba2-A90C-FCF8BCD9CFAF}
DEFINE_GUID(IID_IDirect3DMobileSurface, 
0x496a8f01, 0x1842, 0x4ba2, 0xa9, 0xc, 0xfc, 0xf8, 0xbc, 0xd9, 0xcf, 0xaf);

// IID_IDirect3DMobileSwapChain
// {F593ED9C-024A-400b-9D33-8D19ED408DF7}
DEFINE_GUID(IID_IDirect3DMobileSwapChain, 
0xf593ed9c, 0x24a, 0x400b, 0x9d, 0x33, 0x8d, 0x19, 0xed, 0x40, 0x8d, 0xf7);

interface IDirect3DMobile;
interface IDirect3DMobileDevice;
interface IDirect3DMobileResource;
interface IDirect3DMobileBaseTexture;
interface IDirect3DMobileTexture;
interface IDirect3DMobileVertexBuffer;
interface IDirect3DMobileIndexBuffer;
interface IDirect3DMobileSurface;
interface IDirect3DMobileSwapChain;

typedef interface IDirect3DMobile                IDirect3DMobile;
typedef interface IDirect3DMobileDevice          IDirect3DMobileDevice;
typedef interface IDirect3DMobileResource        IDirect3DMobileResource;
typedef interface IDirect3DMobileBaseTexture     IDirect3DMobileBaseTexture;
typedef interface IDirect3DMobileTexture         IDirect3DMobileTexture;
typedef interface IDirect3DMobileVertexBuffer    IDirect3DMobileVertexBuffer;
typedef interface IDirect3DMobileIndexBuffer     IDirect3DMobileIndexBuffer;
typedef interface IDirect3DMobileSurface         IDirect3DMobileSurface;
typedef interface IDirect3DMobileSwapChain       IDirect3DMobileSwapChain;

#include "d3dmtypes.h"
#include "d3dmcaps.h"

#ifdef __cplusplus
extern "C" {
#endif

// DLL Function for creating a Direct3DMobile object. This object supports
// enumeration and allows the creation of Direct3DMobileDevice objects.
// Pass the value of the constant D3D_SDK_VERSION to this function, so
// that the run-time can validate that your application was compiled
// against the right headers.

IDirect3DMobile * WINAPI Direct3DMobileCreate(UINT SDKVersion);

// Direct3DMobile interfaces

#undef INTERFACE
#define INTERFACE IDirect3DMobile

DECLARE_INTERFACE_(IDirect3DMobile, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobile methods
    STDMETHOD(RegisterSoftwareDevice)(THIS_ VOID* pInitializeFunction) PURE;
    STDMETHOD_(UINT, GetAdapterCount)(THIS) PURE;
    STDMETHOD(GetAdapterIdentifier)(THIS_ UINT Adapter,ULONG Flags,D3DMADAPTER_IDENTIFIER* pIdentifier) PURE;
    STDMETHOD_(UINT, GetAdapterModeCount)(THIS_ UINT Adapter) PURE;
    STDMETHOD(EnumAdapterModes)(THIS_ UINT Adapter,UINT Mode,D3DMDISPLAYMODE* pMode) PURE;
    STDMETHOD(GetAdapterDisplayMode)(THIS_ UINT Adapter,D3DMDISPLAYMODE* pMode) PURE;
    STDMETHOD(CheckDeviceType)(THIS_ UINT Adapter,D3DMDEVTYPE CheckType,D3DMFORMAT DisplayFormat,D3DMFORMAT BackBufferFormat,BOOL Windowed) PURE;
    STDMETHOD(CheckDeviceFormat)(THIS_ UINT Adapter,D3DMDEVTYPE DeviceType,D3DMFORMAT AdapterFormat,ULONG Usage,D3DMRESOURCETYPE RType,D3DMFORMAT CheckFormat) PURE;
    STDMETHOD(CheckDeviceMultiSampleType)(THIS_ UINT Adapter,D3DMDEVTYPE DeviceType,D3DMFORMAT SurfaceFormat,BOOL Windowed,D3DMMULTISAMPLE_TYPE MultiSampleType) PURE;
    STDMETHOD(CheckDepthStencilMatch)(THIS_ UINT Adapter,D3DMDEVTYPE DeviceType,D3DMFORMAT AdapterFormat,D3DMFORMAT RenderTargetFormat,D3DMFORMAT DepthStencilFormat) PURE;
	STDMETHOD(CheckProfile)(THIS_ UINT Adapter, D3DMDEVTYPE DeviceType, D3DMPROFILE Profile) PURE;
    STDMETHOD(CheckDeviceFormatConversion)(THIS_ UINT Adapter, D3DMDEVTYPE DeviceType, D3DMFORMAT SourceFormat, D3DMFORMAT DestFormat) PURE;
    STDMETHOD(GetDeviceCaps)(THIS_ UINT Adapter,D3DMDEVTYPE DeviceType,D3DMCAPS* pCaps) PURE;
    STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DMDEVTYPE DeviceType,HWND hFocusWindow,ULONG BehaviorFlags,D3DMPRESENT_PARAMETERS* pPresentationParameters,IDirect3DMobileDevice** ppReturnedDeviceInterface) PURE;
};

typedef struct IDirect3DMobile *LPDIRECT3DMOBILE, *PDIRECT3DMOBILE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobile_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobile_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobile_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobile_RegisterSoftwareDevice(p,a) (p)->lpVtbl->RegisterSoftwareDevice(p,a)
#define IDirect3DMobile_GetAdapterCount(p) (p)->lpVtbl->GetAdapterCount(p)
#define IDirect3DMobile_GetAdapterIdentifier(p,a,b,c) (p)->lpVtbl->GetAdapterIdentifier(p,a,b,c)
#define IDirect3DMobile_GetAdapterModeCount(p,a) (p)->lpVtbl->GetAdapterModeCount(p,a)
#define IDirect3DMobile_EnumAdapterModes(p,a,b,c) (p)->lpVtbl->EnumAdapterModes(p,a,b,c)
#define IDirect3DMobile_GetAdapterDisplayMode(p,a,b) (p)->lpVtbl->GetAdapterDisplayMode(p,a,b)
#define IDirect3DMobile_CheckDeviceType(p,a,b,c,d,e) (p)->lpVtbl->CheckDeviceType(p,a,b,c,d,e)
#define IDirect3DMobile_CheckDeviceFormat(p,a,b,c,d,e,f) (p)->lpVtbl->CheckDeviceFormat(p,a,b,c,d,e,f)
#define IDirect3DMobile_CheckDeviceMultiSampleType(p,a,b,c,d,e) (p)->lpVtbl->CheckDeviceMultiSampleType(p,a,b,c,d,e)
#define IDirect3DMobile_CheckDepthStencilMatch(p,a,b,c,d,e) (p)->lpVtbl->CheckDepthStencilMatch(p,a,b,c,d,e)
#define IDirect3DMobile_CheckProfile(p,a,b,c) (p)->lpVtbl->CheckProfile(p,a,b,c)
#define IDIrect3DMobile_CheckDeviceFormatConversion(p,a,b,c,d) (p)->lpVtbl->CheckDeviceFormatConversion(p,a,b,c,d)
#define IDirect3DMobile_GetDeviceCaps(p,a,b,c) (p)->lpVtbl->GetDeviceCaps(p,a,b,c)
#define IDirect3DMobile_CreateDevice(p,a,b,c,d,e,f) (p)->lpVtbl->CreateDevice(p,a,b,c,d,e,f)
#else
#define IDirect3DMobile_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobile_AddRef(p) (p)->AddRef()
#define IDirect3DMobile_Release(p) (p)->Release()
#define IDirect3DMobile_RegisterSoftwareDevice(p,a) (p)->RegisterSoftwareDevice(a)
#define IDirect3DMobile_GetAdapterCount(p) (p)->GetAdapterCount()
#define IDirect3DMobile_GetAdapterIdentifier(p,a,b,c) (p)->GetAdapterIdentifier(a,b,c)
#define IDirect3DMobile_GetAdapterModeCount(p,a) (p)->GetAdapterModeCount(a)
#define IDirect3DMobile_EnumAdapterModes(p,a,b,c) (p)->EnumAdapterModes(a,b,c)
#define IDirect3DMobile_GetAdapterDisplayMode(p,a,b) (p)->GetAdapterDisplayMode(a,b)
#define IDirect3DMobile_CheckDeviceType(p,a,b,c,d,e) (p)->CheckDeviceType(a,b,c,d,e)
#define IDirect3DMobile_CheckDeviceFormat(p,a,b,c,d,e,f) (p)->CheckDeviceFormat(a,b,c,d,e,f)
#define IDirect3DMobile_CheckDeviceMultiSampleType(p,a,b,c,d,e) (p)->CheckDeviceMultiSampleType(a,b,c,d,e)
#define IDirect3DMobile_CheckDepthStencilMatch(p,a,b,c,d,e) (p)->CheckDepthStencilMatch(a,b,c,d,e)
#define IDirect3DMobile_CheckProfile(p,a,b,c) (p)->CheckProfile(a,b,c)
#define IDIrect3DMobile_CheckDeviceFormatConversion(p,a,b,c,d) (p)->CheckDeviceFormatConversion(a,b,c,d)
#define IDirect3DMobile_GetDeviceCaps(p,a,b,c) (p)->GetDeviceCaps(a,b,c)
#define IDirect3DMobile_CreateDevice(p,a,b,c,d,e,f) (p)->CreateDevice(a,b,c,d,e,f)
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileDevice

DECLARE_INTERFACE_(IDirect3DMobileDevice, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileDevice methods
    STDMETHOD(TestCooperativeLevel)(THIS) PURE;
    STDMETHOD_(UINT, GetAvailablePoolMem)(THIS_ D3DMPOOL Pool) PURE;
    STDMETHOD(ResourceManagerDiscardBytes)(THIS_ DWORD Bytes) PURE;
    STDMETHOD(GetDirect3D)(THIS_ IDirect3DMobile** ppD3DM) PURE;
    STDMETHOD(GetDeviceCaps)(THIS_ D3DMCAPS* pCaps) PURE;
    STDMETHOD(GetDisplayMode)(THIS_ D3DMDISPLAYMODE* pMode) PURE;
    STDMETHOD(GetCreationParameters)(THIS_ D3DMDEVICE_CREATION_PARAMETERS *pParameters) PURE;
    STDMETHOD(CreateAdditionalSwapChain)(THIS_ D3DMPRESENT_PARAMETERS* pPresentationParameters,IDirect3DMobileSwapChain** pSwapChain) PURE;
    STDMETHOD(Reset)(THIS_ D3DMPRESENT_PARAMETERS* pPresentationParameters) PURE;
    STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) PURE;
    STDMETHOD(GetBackBuffer)(THIS_ UINT BackBuffer,D3DMBACKBUFFER_TYPE Type,IDirect3DMobileSurface** ppBackBuffer) PURE;
    STDMETHOD(CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DMFORMAT Format,D3DMPOOL Pool,IDirect3DMobileTexture** ppTexture) PURE;
    STDMETHOD(CreateVertexBuffer)(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DMPOOL Pool,IDirect3DMobileVertexBuffer** ppVertexBuffer) PURE;
    STDMETHOD(CreateIndexBuffer)(THIS_ UINT Length,DWORD Usage,D3DMFORMAT Format,D3DMPOOL Pool,IDirect3DMobileIndexBuffer** ppIndexBuffer) PURE;
    STDMETHOD(CreateRenderTarget)(THIS_ UINT Width,UINT Height,D3DMFORMAT Format,D3DMMULTISAMPLE_TYPE MultiSample,BOOL Lockable,IDirect3DMobileSurface** ppSurface) PURE;
    STDMETHOD(CreateDepthStencilSurface)(THIS_ UINT Width,UINT Height,D3DMFORMAT Format,D3DMMULTISAMPLE_TYPE MultiSample,IDirect3DMobileSurface** ppSurface) PURE;
    STDMETHOD(CreateImageSurface)(THIS_ UINT Width,UINT Height,D3DMFORMAT Format,IDirect3DMobileSurface** ppSurface) PURE;
    STDMETHOD(CopyRects)(THIS_ IDirect3DMobileSurface* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DMobileSurface* pDestinationSurface,CONST POINT* pDestPointsArray) PURE;
    STDMETHOD(UpdateTexture)(THIS_ IDirect3DMobileBaseTexture* pSourceTexture,IDirect3DMobileBaseTexture* pDestinationTexture) PURE;
    STDMETHOD(GetFrontBuffer)(THIS_ IDirect3DMobileSurface* pDestSurface) PURE;
    STDMETHOD(SetRenderTarget)(THIS_ IDirect3DMobileSurface* pRenderTarget,IDirect3DMobileSurface* pNewZStencil) PURE;
    STDMETHOD(GetRenderTarget)(THIS_ IDirect3DMobileSurface** ppRenderTarget) PURE;
    STDMETHOD(GetDepthStencilSurface)(THIS_ IDirect3DMobileSurface** ppZStencilSurface) PURE;
    STDMETHOD(BeginScene)(THIS) PURE;
    STDMETHOD(EndScene)(THIS) PURE;
    STDMETHOD(Clear)(THIS_ DWORD Count,CONST RECT* pRects,DWORD Flags,D3DMCOLOR Color,float Z,DWORD Stencil) PURE;
    STDMETHOD(SetTransform)(THIS_ D3DMTRANSFORMSTATETYPE State,CONST D3DMMATRIX* pMatrix, D3DMFORMAT Format) PURE;
    STDMETHOD(GetTransform)(THIS_ D3DMTRANSFORMSTATETYPE State,D3DMMATRIX* pMatrix, D3DMFORMAT* pFormat) PURE;
    STDMETHOD(SetViewport)(THIS_ CONST D3DMVIEWPORT* pViewport) PURE;
    STDMETHOD(GetViewport)(THIS_ D3DMVIEWPORT* pViewport) PURE;
    STDMETHOD(SetMaterial)(THIS_ CONST D3DMMATERIAL* pMaterial, D3DMFORMAT Format) PURE;
    STDMETHOD(GetMaterial)(THIS_ D3DMMATERIAL* pMaterial, D3DMFORMAT* pFormat) PURE;
    STDMETHOD(SetLight)(THIS_ DWORD Index,CONST D3DMLIGHT* pLight, D3DMFORMAT Format) PURE;
    STDMETHOD(GetLight)(THIS_ DWORD Index,D3DMLIGHT* pLight, D3DMFORMAT* pFormat) PURE;
    STDMETHOD(LightEnable)(THIS_ DWORD Index,BOOL Enable) PURE;
    STDMETHOD(GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable) PURE;
    STDMETHOD(SetRenderState)(THIS_ D3DMRENDERSTATETYPE State,DWORD Value) PURE;
    STDMETHOD(GetRenderState)(THIS_ D3DMRENDERSTATETYPE State,DWORD* pValue) PURE;
    STDMETHOD(SetClipStatus)(THIS_ CONST D3DMCLIPSTATUS* pClipStatus) PURE;
    STDMETHOD(GetClipStatus)(THIS_ D3DMCLIPSTATUS* pClipStatus) PURE;
    STDMETHOD(GetTexture)(THIS_ DWORD Stage,IDirect3DMobileBaseTexture** ppTexture) PURE;
    STDMETHOD(SetTexture)(THIS_ DWORD Stage,IDirect3DMobileBaseTexture* pTexture) PURE;
    STDMETHOD(GetTextureStageState)(THIS_ DWORD Stage,D3DMTEXTURESTAGESTATETYPE Type,DWORD* pValue) PURE;
    STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage,D3DMTEXTURESTAGESTATETYPE Type,DWORD Value) PURE;
    STDMETHOD(ValidateDevice)(THIS_ DWORD* pNumPasses) PURE;
    STDMETHOD(GetInfo)(THIS_ DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize) PURE;
    STDMETHOD(SetPaletteEntries)(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries) PURE;
    STDMETHOD(GetPaletteEntries)(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries) PURE;
    STDMETHOD(SetCurrentTexturePalette)(THIS_ UINT PaletteNumber) PURE;
    STDMETHOD(GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber) PURE;
    STDMETHOD(DrawPrimitive)(THIS_ D3DMPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) PURE;
    STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DMPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount) PURE;
    STDMETHOD(ProcessVertices)(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DMobileVertexBuffer* pDestBuffer,DWORD Flags) PURE;
    STDMETHOD(SetStreamSource)(THIS_ UINT StreamNumber,IDirect3DMobileVertexBuffer* pStreamData,UINT Stride) PURE;
    STDMETHOD(GetStreamSource)(THIS_ UINT StreamNumber,IDirect3DMobileVertexBuffer** ppStreamData,UINT* pStride) PURE;
    STDMETHOD(SetIndices)(THIS_ IDirect3DMobileIndexBuffer* pIndexData) PURE;
    STDMETHOD(GetIndices)(THIS_ IDirect3DMobileIndexBuffer** ppIndexData) PURE;
	STDMETHOD(StretchRect)(THIS_ IDirect3DMobileSurface* pSourceSurface,CONST RECT* pSourceRect,IDirect3DMobileSurface* pDestSurface,CONST RECT* pDestRect,D3DMTEXTUREFILTERTYPE Filter) PURE;
    STDMETHOD(ColorFill)(THIS_ IDirect3DMobileSurface* pSurface,CONST RECT* pRect,D3DMCOLOR Color) PURE;
};

typedef struct IDirect3DMobileDevice *LPDIRECT3DMOBILEDEVICE, *PDIRECT3DMOBILEDEVICE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileDevice_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileDevice_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileDevice_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileDevice_TestCooperativeLevel(p) (p)->lpVtbl->TestCooperativeLevel(p)
#define IDirect3DMobileDevice_GetAvailablePoolMem(p,a) (p)->lpVtbl->GetAvailablePoolMem(p,a)
#define IDirect3DMobileDevice_ResourceManagerDiscardBytes(p,a) (p)->lpVtbl->ResourceManagerDiscardBytes(p,a)
#define IDirect3DMobileDevice_GetDirect3D(p,a) (p)->lpVtbl->GetDirect3D(p,a)
#define IDirect3DMobileDevice_GetDeviceCaps(p,a) (p)->lpVtbl->GetDeviceCaps(p,a)
#define IDirect3DMobileDevice_GetDisplayMode(p,a) (p)->lpVtbl->GetDisplayMode(p,a)
#define IDirect3DMobileDevice_GetCreationParameters(p,a) (p)->lpVtbl->GetCreationParameters(p,a)
#define IDirect3DMobileDevice_CreateAdditionalSwapChain(p,a,b) (p)->lpVtbl->CreateAdditionalSwapChain(p,a,b)
#define IDirect3DMobileDevice_Reset(p,a) (p)->lpVtbl->Reset(p,a)
#define IDirect3DMobileDevice_Present(p,a,b,c,d) (p)->lpVtbl->Present(p,a,b,c,d)
#define IDirect3DMobileDevice_GetBackBuffer(p,a,b,c) (p)->lpVtbl->GetBackBuffer(p,a,b,c)
#define IDirect3DMobileDevice_CreateTexture(p,a,b,c,d,e,f,g) (p)->lpVtbl->CreateTexture(p,a,b,c,d,e,f,g)
#define IDirect3DMobileDevice_CreateVertexBuffer(p,a,b,c,d,e) (p)->lpVtbl->CreateVertexBuffer(p,a,b,c,d,e)
#define IDirect3DMobileDevice_CreateIndexBuffer(p,a,b,c,d,e) (p)->lpVtbl->CreateIndexBuffer(p,a,b,c,d,e)
#define IDirect3DMobileDevice_CreateRenderTarget(p,a,b,c,d,e,f) (p)->lpVtbl->CreateRenderTarget(p,a,b,c,d,e,f)
#define IDirect3DMobileDevice_CreateDepthStencilSurface(p,a,b,c,d,e) (p)->lpVtbl->CreateDepthStencilSurface(p,a,b,c,d,e)
#define IDirect3DMobileDevice_CreateImageSurface(p,a,b,c,d) (p)->lpVtbl->CreateImageSurface(p,a,b,c,d)
#define IDirect3DMobileDevice_CopyRects(p,a,b,c,d,e) (p)->lpVtbl->CopyRects(p,a,b,c,d,e)
#define IDirect3DMobileDevice_UpdateTexture(p,a,b) (p)->lpVtbl->UpdateTexture(p,a,b)
#define IDirect3DMobileDevice_GetFrontBuffer(p,a) (p)->lpVtbl->GetFrontBuffer(p,a)
#define IDirect3DMobileDevice_SetRenderTarget(p,a,b) (p)->lpVtbl->SetRenderTarget(p,a,b)
#define IDirect3DMobileDevice_GetRenderTarget(p,a) (p)->lpVtbl->GetRenderTarget(p,a)
#define IDirect3DMobileDevice_GetDepthStencilSurface(p,a) (p)->lpVtbl->GetDepthStencilSurface(p,a)
#define IDirect3DMobileDevice_BeginScene(p) (p)->lpVtbl->BeginScene(p)
#define IDirect3DMobileDevice_EndScene(p) (p)->lpVtbl->EndScene(p)
#define IDirect3DMobileDevice_Clear(p,a,b,c,d,e,f) (p)->lpVtbl->Clear(p,a,b,c,d,e,f)
#define IDirect3DMobileDevice_SetTransform(p,a,b,c) (p)->lpVtbl->SetTransform(p,a,b,c)
#define IDirect3DMobileDevice_GetTransform(p,a,b,c) (p)->lpVtbl->GetTransform(p,a,b,c)
#define IDirect3DMobileDevice_SetViewport(p,a,b) (p)->lpVtbl->SetViewport(p,a,b)
#define IDirect3DMobileDevice_GetViewport(p,a,b) (p)->lpVtbl->GetViewport(p,a,b)
#define IDirect3DMobileDevice_SetMaterial(p,a,b) (p)->lpVtbl->SetMaterial(p,a,b)
#define IDirect3DMobileDevice_GetMaterial(p,a,b) (p)->lpVtbl->GetMaterial(p,a,b)
#define IDirect3DMobileDevice_SetLight(p,a,b,c) (p)->lpVtbl->SetLight(p,a,b,c)
#define IDirect3DMobileDevice_GetLight(p,a,b,c) (p)->lpVtbl->GetLight(p,a,b,c)
#define IDirect3DMobileDevice_LightEnable(p,a,b) (p)->lpVtbl->LightEnable(p,a,b)
#define IDirect3DMobileDevice_GetLightEnable(p,a,b) (p)->lpVtbl->GetLightEnable(p,a,b)
#define IDirect3DMobileDevice_SetRenderState(p,a,b) (p)->lpVtbl->SetRenderState(p,a,b)
#define IDirect3DMobileDevice_GetRenderState(p,a,b) (p)->lpVtbl->GetRenderState(p,a,b)
#define IDirect3DMobileDevice_SetClipStatus(p,a) (p)->lpVtbl->SetClipStatus(p,a)
#define IDirect3DMobileDevice_GetClipStatus(p,a) (p)->lpVtbl->GetClipStatus(p,a)
#define IDirect3DMobileDevice_GetTexture(p,a,b) (p)->lpVtbl->GetTexture(p,a,b)
#define IDirect3DMobileDevice_SetTexture(p,a,b) (p)->lpVtbl->SetTexture(p,a,b)
#define IDirect3DMobileDevice_GetTextureStageState(p,a,b,c) (p)->lpVtbl->GetTextureStageState(p,a,b,c)
#define IDirect3DMobileDevice_SetTextureStageState(p,a,b,c) (p)->lpVtbl->SetTextureStageState(p,a,b,c)
#define IDirect3DMobileDevice_ValidateDevice(p,a) (p)->lpVtbl->ValidateDevice(p,a)
#define IDirect3DMobileDevice_GetInfo(p,a,b,c) (p)->lpVtbl->GetInfo(p,a,b,c)
#define IDirect3DMobileDevice_SetPaletteEntries(p,a,b) (p)->lpVtbl->SetPaletteEntries(p,a,b)
#define IDirect3DMobileDevice_GetPaletteEntries(p,a,b) (p)->lpVtbl->GetPaletteEntries(p,a,b)
#define IDirect3DMobileDevice_SetCurrentTexturePalette(p,a) (p)->lpVtbl->SetCurrentTexturePalette(p,a)
#define IDirect3DMobileDevice_GetCurrentTexturePalette(p,a) (p)->lpVtbl->GetCurrentTexturePalette(p,a)
#define IDirect3DMobileDevice_DrawPrimitive(p,a,b,c) (p)->lpVtbl->DrawPrimitive(p,a,b,c)
#define IDirect3DMobileDevice_DrawIndexedPrimitive(p,a,b,c,d,e,f) (p)->lpVtbl->DrawIndexedPrimitive(p,a,b,c,d,e,f)
#define IDirect3DMobileDevice_ProcessVertices(p,a,b,c,d,e) (p)->lpVtbl->ProcessVertices(p,a,b,c,d,e)
#define IDirect3DMobileDevice_SetStreamSource(p,a,b,c) (p)->lpVtbl->SetStreamSource(p,a,b,c)
#define IDirect3DMobileDevice_GetStreamSource(p,a,b,c) (p)->lpVtbl->GetStreamSource(p,a,b,c)
#define IDirect3DMobileDevice_SetIndices(p,a) (p)->lpVtbl->SetIndices(p,a)
#define IDirect3DMobileDevice_GetIndices(p,a) (p)->lpVtbl->GetIndices(p,a)
#define IDirect3DMobileDevice_StretchRect(p,a,b,c,d,e) (p)->lpVtbl->StretchRect(p,a,b,c,d,e)
#define IDirect3DMobileDevice_ColorFill(p,a,b,c) (p)->lpVtbl->ColorFill(p,a,b,c)
#else
#define IDirect3DMobileDevice_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileDevice_AddRef(p) (p)->AddRef()
#define IDirect3DMobileDevice_Release(p) (p)->Release()
#define IDirect3DMobileDevice_TestCooperativeLevel(p) (p)->TestCooperativeLevel()
#define IDirect3DMobileDevice_GetAvailablePoolMem(p,a) (p)->GetAvailablePoolMem(a)
#define IDirect3DMobileDevice_ResourceManagerDiscardBytes(p,a) (p)->ResourceManagerDiscardBytes(a)
#define IDirect3DMobileDevice_GetDirect3D(p,a) (p)->GetDirect3D(a)
#define IDirect3DMobileDevice_GetDeviceCaps(p,a) (p)->GetDeviceCaps(a)
#define IDirect3DMobileDevice_GetDisplayMode(p,a) (p)->GetDisplayMode(a)
#define IDirect3DMobileDevice_GetCreationParameters(p,a) (p)->GetCreationParameters(a)
#define IDirect3DMobileDevice_CreateAdditionalSwapChain(p,a,b) (p)->CreateAdditionalSwapChain(a,b)
#define IDirect3DMobileDevice_Reset(p,a) (p)->Reset(a)
#define IDirect3DMobileDevice_Present(p,a,b,c,d) (p)->Present(a,b,c,d)
#define IDirect3DMobileDevice_GetBackBuffer(p,a,b,c) (p)->GetBackBuffer(a,b,c)
#define IDirect3DMobileDevice_CreateTexture(p,a,b,c,d,e,f,g) (p)->CreateTexture(a,b,c,d,e,f,g)
#define IDirect3DMobileDevice_CreateVertexBuffer(p,a,b,c,d,e) (p)->CreateVertexBuffer(a,b,c,d,e)
#define IDirect3DMobileDevice_CreateIndexBuffer(p,a,b,c,d,e) (p)->CreateIndexBuffer(a,b,c,d,e)
#define IDirect3DMobileDevice_CreateRenderTarget(p,a,b,c,d,e,f) (p)->CreateRenderTarget(a,b,c,d,e,f)
#define IDirect3DMobileDevice_CreateDepthStencilSurface(p,a,b,c,d,e) (p)->CreateDepthStencilSurface(a,b,c,d,e)
#define IDirect3DMobileDevice_CreateImageSurface(p,a,b,c,d) (p)->CreateImageSurface(a,b,c,d)
#define IDirect3DMobileDevice_CopyRects(p,a,b,c,d,e) (p)->CopyRects(a,b,c,d,e)
#define IDirect3DMobileDevice_UpdateTexture(p,a,b) (p)->UpdateTexture(a,b)
#define IDirect3DMobileDevice_GetFrontBuffer(p,a) (p)->GetFrontBuffer(a)
#define IDirect3DMobileDevice_SetRenderTarget(p,a,b) (p)->SetRenderTarget(a,b)
#define IDirect3DMobileDevice_GetRenderTarget(p,a) (p)->GetRenderTarget(a)
#define IDirect3DMobileDevice_GetDepthStencilSurface(p,a) (p)->GetDepthStencilSurface(a)
#define IDirect3DMobileDevice_BeginScene(p) (p)->BeginScene()
#define IDirect3DMobileDevice_EndScene(p) (p)->EndScene()
#define IDirect3DMobileDevice_Clear(p,a,b,c,d,e,f) (p)->Clear(a,b,c,d,e,f)
#define IDirect3DMobileDevice_SetTransform(p,a,b,c) (p)->SetTransform(a,b,c)
#define IDirect3DMobileDevice_GetTransform(p,a,b,c) (p)->GetTransform(a,b,c)
#define IDirect3DMobileDevice_SetViewport(p,a,b) (p)->SetViewport(a,b)
#define IDirect3DMobileDevice_GetViewport(p,a,b) (p)->GetViewport(a,b)
#define IDirect3DMobileDevice_SetMaterial(p,a,b) (p)->SetMaterial(a,b)
#define IDirect3DMobileDevice_GetMaterial(p,a,b) (p)->GetMaterial(a,b)
#define IDirect3DMobileDevice_SetLight(p,a,b,c) (p)->SetLight(a,b,c)
#define IDirect3DMobileDevice_GetLight(p,a,b,c) (p)->GetLight(a,b,c)
#define IDirect3DMobileDevice_LightEnable(p,a,b) (p)->LightEnable(a,b)
#define IDirect3DMobileDevice_GetLightEnable(p,a,b) (p)->GetLightEnable(a,b)
#define IDirect3DMobileDevice_SetRenderState(p,a,b) (p)->SetRenderState(a,b)
#define IDirect3DMobileDevice_GetRenderState(p,a,b) (p)->GetRenderState(a,b)
#define IDirect3DMobileDevice_SetClipStatus(p,a) (p)->SetClipStatus(a)
#define IDirect3DMobileDevice_GetClipStatus(p,a) (p)->GetClipStatus(a)
#define IDirect3DMobileDevice_GetTexture(p,a,b) (p)->GetTexture(a,b)
#define IDirect3DMobileDevice_SetTexture(p,a,b) (p)->SetTexture(a,b)
#define IDirect3DMobileDevice_GetTextureStageState(p,a,b,c) (p)->GetTextureStageState(a,b,c)
#define IDirect3DMobileDevice_SetTextureStageState(p,a,b,c) (p)->SetTextureStageState(a,b,c)
#define IDirect3DMobileDevice_ValidateDevice(p,a) (p)->ValidateDevice(a)
#define IDirect3DMobileDevice_GetInfo(p,a,b,c) (p)->GetInfo(a,b,c)
#define IDirect3DMobileDevice_SetPaletteEntries(p,a,b) (p)->SetPaletteEntries(a,b)
#define IDirect3DMobileDevice_GetPaletteEntries(p,a,b) (p)->GetPaletteEntries(a,b)
#define IDirect3DMobileDevice_SetCurrentTexturePalette(p,a) (p)->SetCurrentTexturePalette(a)
#define IDirect3DMobileDevice_GetCurrentTexturePalette(p,a) (p)->GetCurrentTexturePalette(a)
#define IDirect3DMobileDevice_DrawPrimitive(p,a,b,c) (p)->DrawPrimitive(a,b,c)
#define IDirect3DMobileDevice_DrawIndexedPrimitive(p,a,b,c,d,e,f) (p)->DrawIndexedPrimitive(a,b,c,d,e,f)
#define IDirect3DMobileDevice_ProcessVertices(p,a,b,c,d,e) (p)->ProcessVertices(a,b,c,d,e)
#define IDirect3DMobileDevice_SetStreamSource(p,a,b,c) (p)->SetStreamSource(a,b,c)
#define IDirect3DMobileDevice_GetStreamSource(p,a,b,c) (p)->GetStreamSource(a,b,c)
#define IDirect3DMobileDevice_SetIndices(p,a) (p)->SetIndices(a)
#define IDirect3DMobileDevice_GetIndices(p,a) (p)->GetIndices(a)
#define IDirect3DMobileDevice_StretchRect(p,a,b,c,d,e) (p)->StretchRect(a,b,c,d,e)
#define IDirect3DMobileDevice_ColorFill(p,a,b,c) (p)->ColorFill(a,b,c)
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileSwapChain

DECLARE_INTERFACE_(IDirect3DMobileSwapChain, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileSwapChain methods
    STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) PURE;
    STDMETHOD(GetBackBuffer)(THIS_ UINT BackBuffer,D3DMBACKBUFFER_TYPE Type,IDirect3DMobileSurface** ppBackBuffer) PURE;
};

typedef struct IDirect3DMobileSwapChain *LPDIRECT3DMOBILESWAPCHAIN, *PDIRECT3DMOBILESWAPCHAIN;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileSwapChain_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileSwapChain_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileSwapChain_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileSwapChain_Present(p,a,b,c,d) (p)->lpVtbl->Present(p,a,b,c,d)
#define IDirect3DMobileSwapChain_GetBackBuffer(p,a,b,c) (p)->lpVtbl->GetBackBuffer(p,a,b,c)
#else
#define IDirect3DMobileSwapChain_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileSwapChain_AddRef(p) (p)->AddRef()
#define IDirect3DMobileSwapChain_Release(p) (p)->Release()
#define IDirect3DMobileSwapChain_Present(p,a,b,c,d) (p)->Present(a,b,c,d)
#define IDirect3DMobileSwapChain_GetBackBuffer(p,a,b,c) (p)->GetBackBuffer(a,b,c)
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileResource

DECLARE_INTERFACE_(IDirect3DMobileResource, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileResource methods
    STDMETHOD(GetDevice)(THIS_ IDirect3DMobileDevice** ppDevice) PURE;
    STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
    STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
    STDMETHOD_(void, PreLoad)(THIS) PURE;
    STDMETHOD_(D3DMRESOURCETYPE, GetType)(THIS) PURE;
};

typedef struct IDirect3DMobileResource *LPDIRECT3DMOBILERESOURCE, *PDIRECT3DMOBILERESOURCE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileResource_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileResource_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileResource_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileResource_GetDevice(p,a) (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DMobileResource_SetPriority(p,a) (p)->lpVtbl->SetPriority(p,a)
#define IDirect3DMobileResource_GetPriority(p) (p)->lpVtbl->GetPriority(p)
#define IDirect3DMobileResource_PreLoad(p) (p)->lpVtbl->PreLoad(p)
#define IDirect3DMobileResource_GetType(p) (p)->lpVtbl->GetType(p)
#else
#define IDirect3DMobileResource_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileResource_AddRef(p) (p)->AddRef()
#define IDirect3DMobileResource_Release(p) (p)->Release()
#define IDirect3DMobileResource_GetDevice(p,a) (p)->GetDevice(a)
#define IDirect3DMobileResource_SetPriority(p,a) (p)->SetPriority(a)
#define IDirect3DMobileResource_GetPriority(p) (p)->GetPriority()
#define IDirect3DMobileResource_PreLoad(p) (p)->PreLoad()
#define IDirect3DMobileResource_GetType(p) (p)->GetType()
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileBaseTexture

DECLARE_INTERFACE_(IDirect3DMobileBaseTexture, IDirect3DMobileResource)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileResource methods
    STDMETHOD(GetDevice)(THIS_ IDirect3DMobileDevice** ppDevice) PURE;
    STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
    STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
    STDMETHOD_(void, PreLoad)(THIS) PURE;
    STDMETHOD_(D3DMRESOURCETYPE, GetType)(THIS) PURE;

    // IDirect3DMobileBaseTexture methods
    STDMETHOD_(DWORD, SetLOD)(THIS_ DWORD LODNew) PURE;
    STDMETHOD_(DWORD, GetLOD)(THIS) PURE;
    STDMETHOD_(DWORD, GetLevelCount)(THIS) PURE;
};

typedef struct IDirect3DMobileBaseTexture *LPDIRECT3DMOBILEBASETEXTURE, *PDIRECT3DMOBILEBASETEXTURE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileBaseTexture_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileBaseTexture_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileBaseTexture_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileBaseTexture_GetDevice(p,a) (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DMobileBaseTexture_SetPriority(p,a) (p)->lpVtbl->SetPriority(p,a)
#define IDirect3DMobileBaseTexture_GetPriority(p) (p)->lpVtbl->GetPriority(p)
#define IDirect3DMobileBaseTexture_PreLoad(p) (p)->lpVtbl->PreLoad(p)
#define IDirect3DMobileBaseTexture_GetType(p) (p)->lpVtbl->GetType(p)
#define IDirect3DMobileBaseTexture_SetLOD(p,a) (p)->lpVtbl->SetLOD(p,a)
#define IDirect3DMobileBaseTexture_GetLOD(p) (p)->lpVtbl->GetLOD(p)
#define IDirect3DMobileBaseTexture_GetLevelCount(p) (p)->lpVtbl->GetLevelCount(p)
#else
#define IDirect3DMobileBaseTexture_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileBaseTexture_AddRef(p) (p)->AddRef()
#define IDirect3DMobileBaseTexture_Release(p) (p)->Release()
#define IDirect3DMobileBaseTexture_GetDevice(p,a) (p)->GetDevice(a)
#define IDirect3DMobileBaseTexture_SetPriority(p,a) (p)->SetPriority(a)
#define IDirect3DMobileBaseTexture_GetPriority(p) (p)->GetPriority()
#define IDirect3DMobileBaseTexture_PreLoad(p) (p)->PreLoad()
#define IDirect3DMobileBaseTexture_GetType(p) (p)->GetType()
#define IDirect3DMobileBaseTexture_SetLOD(p,a) (p)->SetLOD(a)
#define IDirect3DMobileBaseTexture_GetLOD(p) (p)->GetLOD()
#define IDirect3DMobileBaseTexture_GetLevelCount(p) (p)->GetLevelCount()
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileTexture

DECLARE_INTERFACE_(IDirect3DMobileTexture, IDirect3DMobileBaseTexture)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileResource methods
    STDMETHOD(GetDevice)(THIS_ IDirect3DMobileDevice** ppDevice) PURE;
    STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
    STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
    STDMETHOD_(void, PreLoad)(THIS) PURE;
    STDMETHOD_(D3DMRESOURCETYPE, GetType)(THIS) PURE;

    // IDirect3DMobileBaseTexture methods
    STDMETHOD_(DWORD, SetLOD)(THIS_ DWORD LODNew) PURE;
    STDMETHOD_(DWORD, GetLOD)(THIS) PURE;
    STDMETHOD_(DWORD, GetLevelCount)(THIS) PURE;

    // IDirect3DMobileTexture methods
    STDMETHOD(GetLevelDesc)(THIS_ UINT Level,D3DMSURFACE_DESC *pDesc) PURE;
    STDMETHOD(GetSurfaceLevel)(THIS_ UINT Level,IDirect3DMobileSurface** ppSurfaceLevel) PURE;
    STDMETHOD(LockRect)(THIS_ UINT Level,D3DMLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) PURE;
    STDMETHOD(UnlockRect)(THIS_ UINT Level) PURE;
    STDMETHOD(AddDirtyRect)(THIS_ CONST RECT* pDirtyRect) PURE;
};

typedef struct IDirect3DMobileTexture *LPDIRECT3DMOBILETEXTURE, *PDIRECT3DMOBILETEXTURE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileTexture_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileTexture_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileTexture_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileTexture_GetDevice(p,a) (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DMobileTexture_SetPriority(p,a) (p)->lpVtbl->SetPriority(p,a)
#define IDirect3DMobileTexture_GetPriority(p) (p)->lpVtbl->GetPriority(p)
#define IDirect3DMobileTexture_PreLoad(p) (p)->lpVtbl->PreLoad(p)
#define IDirect3DMobileTexture_GetType(p) (p)->lpVtbl->GetType(p)
#define IDirect3DMobileTexture_SetLOD(p,a) (p)->lpVtbl->SetLOD(p,a)
#define IDirect3DMobileTexture_GetLOD(p) (p)->lpVtbl->GetLOD(p)
#define IDirect3DMobileTexture_GetLevelCount(p) (p)->lpVtbl->GetLevelCount(p)
#define IDirect3DMobileTexture_GetLevelDesc(p,a,b) (p)->lpVtbl->GetLevelDesc(p,a,b)
#define IDirect3DMobileTexture_GetSurfaceLevel(p,a,b) (p)->lpVtbl->GetSurfaceLevel(p,a,b)
#define IDirect3DMobileTexture_LockRect(p,a,b,c,d) (p)->lpVtbl->LockRect(p,a,b,c,d)
#define IDirect3DMobileTexture_UnlockRect(p,a) (p)->lpVtbl->UnlockRect(p,a)
#define IDirect3DMobileTexture_AddDirtyRect(p,a) (p)->lpVtbl->AddDirtyRect(p,a)
#else
#define IDirect3DMobileTexture_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileTexture_AddRef(p) (p)->AddRef()
#define IDirect3DMobileTexture_Release(p) (p)->Release()
#define IDirect3DMobileTexture_GetDevice(p,a) (p)->GetDevice(a)
#define IDirect3DMobileTexture_SetPriority(p,a) (p)->SetPriority(a)
#define IDirect3DMobileTexture_GetPriority(p) (p)->GetPriority()
#define IDirect3DMobileTexture_PreLoad(p) (p)->PreLoad()
#define IDirect3DMobileTexture_GetType(p) (p)->GetType()
#define IDirect3DMobileTexture_SetLOD(p,a) (p)->SetLOD(a)
#define IDirect3DMobileTexture_GetLOD(p) (p)->GetLOD()
#define IDirect3DMobileTexture_GetLevelCount(p) (p)->GetLevelCount()
#define IDirect3DMobileTexture_GetLevelDesc(p,a,b) (p)->GetLevelDesc(a,b)
#define IDirect3DMobileTexture_GetSurfaceLevel(p,a,b) (p)->GetSurfaceLevel(a,b)
#define IDirect3DMobileTexture_LockRect(p,a,b,c,d) (p)->LockRect(a,b,c,d)
#define IDirect3DMobileTexture_UnlockRect(p,a) (p)->UnlockRect(a)
#define IDirect3DMobileTexture_AddDirtyRect(p,a) (p)->AddDirtyRect(a)
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileVertexBuffer

DECLARE_INTERFACE_(IDirect3DMobileVertexBuffer, IDirect3DMobileResource)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileResource methods
    STDMETHOD(GetDevice)(THIS_ IDirect3DMobileDevice** ppDevice) PURE;
    STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
    STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
    STDMETHOD_(void, PreLoad)(THIS) PURE;
    STDMETHOD_(D3DMRESOURCETYPE, GetType)(THIS) PURE;

    // IDirect3DMobileVertexBuffer methods
    STDMETHOD(Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) PURE;
    STDMETHOD(Unlock)(THIS) PURE;
    STDMETHOD(GetDesc)(THIS_ D3DMVERTEXBUFFER_DESC *pDesc) PURE;
};

typedef struct IDirect3DMobileVertexBuffer *LPDIRECT3DMOBILEVERTEXBUFFER, *PDIRECT3DMOBILEVERTEXBUFFER;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileVertexBuffer_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileVertexBuffer_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileVertexBuffer_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileVertexBuffer_GetDevice(p,a) (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DMobileVertexBuffer_SetPriority(p,a) (p)->lpVtbl->SetPriority(p,a)
#define IDirect3DMobileVertexBuffer_GetPriority(p) (p)->lpVtbl->GetPriority(p)
#define IDirect3DMobileVertexBuffer_PreLoad(p) (p)->lpVtbl->PreLoad(p)
#define IDirect3DMobileVertexBuffer_GetType(p) (p)->lpVtbl->GetType(p)
#define IDirect3DMobileVertexBuffer_Lock(p,a,b,c,d) (p)->lpVtbl->Lock(p,a,b,c,d)
#define IDirect3DMobileVertexBuffer_Unlock(p) (p)->lpVtbl->Unlock(p)
#define IDirect3DMobileVertexBuffer_GetDesc(p,a) (p)->lpVtbl->GetDesc(p,a)
#else
#define IDirect3DMobileVertexBuffer_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileVertexBuffer_AddRef(p) (p)->AddRef()
#define IDirect3DMobileVertexBuffer_Release(p) (p)->Release()
#define IDirect3DMobileVertexBuffer_GetDevice(p,a) (p)->GetDevice(a)
#define IDirect3DMobileVertexBuffer_SetPriority(p,a) (p)->SetPriority(a)
#define IDirect3DMobileVertexBuffer_GetPriority(p) (p)->GetPriority()
#define IDirect3DMobileVertexBuffer_PreLoad(p) (p)->PreLoad()
#define IDirect3DMobileVertexBuffer_GetType(p) (p)->GetType()
#define IDirect3DMobileVertexBuffer_Lock(p,a,b,c,d) (p)->Lock(a,b,c,d)
#define IDirect3DMobileVertexBuffer_Unlock(p) (p)->Unlock()
#define IDirect3DMobileVertexBuffer_GetDesc(p,a) (p)->GetDesc(a)
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileIndexBuffer

DECLARE_INTERFACE_(IDirect3DMobileIndexBuffer, IDirect3DMobileResource)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileResource methods
    STDMETHOD(GetDevice)(THIS_ IDirect3DMobileDevice** ppDevice) PURE;
    STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
    STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
    STDMETHOD_(void, PreLoad)(THIS) PURE;
    STDMETHOD_(D3DMRESOURCETYPE, GetType)(THIS) PURE;

    // IDirect3DMobileIndexBuffer
    STDMETHOD(Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) PURE;
    STDMETHOD(Unlock)(THIS) PURE;
    STDMETHOD(GetDesc)(THIS_ D3DMINDEXBUFFER_DESC *pDesc) PURE;
};

typedef struct IDirect3DMobileIndexBuffer *LPDIRECT3DMOBILEINDEXBUFFER, *PDIRECT3DMOBILEINDEXBUFFER;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileIndexBuffer_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileIndexBuffer_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileIndexBuffer_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileIndexBuffer_GetDevice(p,a) (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DMobileIndexBuffer_SetPriority(p,a) (p)->lpVtbl->SetPriority(p,a)
#define IDirect3DMobileIndexBuffer_GetPriority(p) (p)->lpVtbl->GetPriority(p)
#define IDirect3DMobileIndexBuffer_PreLoad(p) (p)->lpVtbl->PreLoad(p)
#define IDirect3DMobileIndexBuffer_GetType(p) (p)->lpVtbl->GetType(p)
#define IDirect3DMobileIndexBuffer_Lock(p,a,b,c,d) (p)->lpVtbl->Lock(p,a,b,c,d)
#define IDirect3DMobileIndexBuffer_Unlock(p) (p)->lpVtbl->Unlock(p)
#define IDirect3DMobileIndexBuffer_GetDesc(p,a) (p)->lpVtbl->GetDesc(p,a)
#else
#define IDirect3DMobileIndexBuffer_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileIndexBuffer_AddRef(p) (p)->AddRef()
#define IDirect3DMobileIndexBuffer_Release(p) (p)->Release()
#define IDirect3DMobileIndexBuffer_GetDevice(p,a) (p)->GetDevice(a)
#define IDirect3DMobileIndexBuffer_SetPriority(p,a) (p)->SetPriority(a)
#define IDirect3DMobileIndexBuffer_GetPriority(p) (p)->GetPriority()
#define IDirect3DMobileIndexBuffer_PreLoad(p) (p)->PreLoad()
#define IDirect3DMobileIndexBuffer_GetType(p) (p)->GetType()
#define IDirect3DMobileIndexBuffer_Lock(p,a,b,c,d) (p)->Lock(a,b,c,d)
#define IDirect3DMobileIndexBuffer_Unlock(p) (p)->Unlock()
#define IDirect3DMobileIndexBuffer_GetDesc(p,a) (p)->GetDesc(a)
#endif

#undef INTERFACE
#define INTERFACE IDirect3DMobileSurface

DECLARE_INTERFACE_(IDirect3DMobileSurface, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IDirect3DMobileSurface methods
    STDMETHOD(GetDevice)(THIS_ IDirect3DMobileDevice** ppDevice) PURE;
    STDMETHOD(GetContainer)(THIS_ REFIID riid,void** ppContainer) PURE;
    STDMETHOD(GetDesc)(THIS_ D3DMSURFACE_DESC *pDesc) PURE;
    STDMETHOD(LockRect)(THIS_ D3DMLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) PURE;
    STDMETHOD(UnlockRect)(THIS) PURE;
	STDMETHOD(GetDC)(THIS_ HDC* phdc) PURE;
	STDMETHOD(ReleaseDC)(THIS_ HDC hdc) PURE;
};

typedef struct IDirect3DMobileSurface *LPDIRECT3DMOBILESURFACE, *PDIRECT3DMOBILESURFACE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirect3DMobileSurface_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirect3DMobileSurface_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirect3DMobileSurface_Release(p) (p)->lpVtbl->Release(p)
#define IDirect3DMobileSurface_GetDevice(p,a) (p)->lpVtbl->GetDevice(p,a)
#define IDirect3DMobileSurface_GetContainer(p,a,b) (p)->lpVtbl->GetContainer(p,a,b)
#define IDirect3DMobileSurface_GetDesc(p,a) (p)->lpVtbl->GetDesc(p,a)
#define IDirect3DMobileSurface_LockRect(p,a,b,c) (p)->lpVtbl->LockRect(p,a,b,c)
#define IDirect3DMobileSurface_UnlockRect(p) (p)->lpVtbl->UnlockRect(p)
#define IDirect3DMobileSurface_GetDC(p,a) (p)->lpVtbl->GetDC(p,a)
#define IDirect3DMobileSurface_ReleaseDC(p,a) (p)->lpVtbl->ReleaseDC(p,a)
#else
#define IDirect3DMobileSurface_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirect3DMobileSurface_AddRef(p) (p)->AddRef()
#define IDirect3DMobileSurface_Release(p) (p)->Release()
#define IDirect3DMobileSurface_GetDevice(p,a) (p)->GetDevice(a)
#define IDirect3DMobileSurface_GetContainer(p,a,b) (p)->GetContainer(a,b)
#define IDirect3DMobileSurface_GetDesc(p,a) (p)->GetDesc(a)
#define IDirect3DMobileSurface_LockRect(p,a,b,c) (p)->LockRect(a,b,c)
#define IDirect3DMobileSurface_UnlockRect(p) (p)->UnlockRect()
#define IDirect3DMobileSurface_GetDC(p,a) (p)->GetDC(a)
#define IDirect3DMobileSurface_ReleaseDC(p,a) (p)->ReleaseDC(a)
#endif

// Parameter for IDirect3DMobile Enum and GetCaps functions to get the info
// for the current mode only.

#define D3DMCURRENT_DISPLAY_MODE                 0x00EFFFFF

// Flags for IDirect3DMobile::CreateDevice's BehaviorFlags

#define D3DMCREATE_MULTITHREADED                 0x00000004

// Parameter for IDirect3DMobile::CreateDevice's Adapter

#define D3DMADAPTER_DEFAULT                      0
#define D3DMADAPTER_REGISTERED_DEVICE            1

// Direct3D Mobile Error Codes

#define _FACD3DM  0xD3D
#define MAKE_D3DMHRESULT( code )  MAKE_HRESULT( 1, _FACD3DM, code )

#define D3DM_OK                                 S_OK
#define D3DM_GENERICFAILURE                     E_FAIL

#define D3DMERR_DRIVERINTERNALERROR             MAKE_D3DMHRESULT(2087)
#define D3DMERR_INVALIDDEVICE                   MAKE_D3DMHRESULT(2155)
#define D3DMERR_INVALIDCALL                     MAKE_D3DMHRESULT(2156)
#define D3DMERR_DRIVERINVALIDCALL               MAKE_D3DMHRESULT(2157)
#define D3DMERR_DRIVERUNSUPPORTED               MAKE_D3DMHRESULT(2088)

#define D3DMERR_WRONGTEXTUREFORMAT              MAKE_D3DMHRESULT(2072)
#define D3DMERR_UNSUPPORTEDCOLOROPERATION       MAKE_D3DMHRESULT(2073)
#define D3DMERR_UNSUPPORTEDCOLORARG             MAKE_D3DMHRESULT(2074)
#define D3DMERR_UNSUPPORTEDALPHAOPERATION       MAKE_D3DMHRESULT(2075)
#define D3DMERR_UNSUPPORTEDALPHAARG             MAKE_D3DMHRESULT(2076)
#define D3DMERR_TOOMANYOPERATIONS               MAKE_D3DMHRESULT(2077)
#define D3DMERR_CONFLICTINGTEXTUREFILTER        MAKE_D3DMHRESULT(2078)
#define D3DMERR_UNSUPPORTEDFACTORVALUE          MAKE_D3DMHRESULT(2079)
#define D3DMERR_CONFLICTINGRENDERSTATE          MAKE_D3DMHRESULT(2081)
#define D3DMERR_UNSUPPORTEDTEXTUREFILTER        MAKE_D3DMHRESULT(2082)
#define D3DMERR_CONFLICTINGTEXTUREPALETTE       MAKE_D3DMHRESULT(2086)

#define D3DMERR_NOTFOUND                        MAKE_D3DMHRESULT(2150)
#define D3DMERR_MOREDATA                        MAKE_D3DMHRESULT(2151)
#define D3DMERR_DEVICELOST                      MAKE_D3DMHRESULT(2152)
#define D3DMERR_DEVICENOTRESET                  MAKE_D3DMHRESULT(2153)
#define D3DMERR_NOTAVAILABLE                    MAKE_D3DMHRESULT(2154)
#define D3DMERR_MEMORYPOOLEMPTY                 MAKE_D3DMHRESULT(380)
#define D3DMERR_ALREADYLOCKED                   MAKE_D3DMHRESULT(2158)

#ifdef __cplusplus
};
#endif
