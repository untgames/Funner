#ifndef RENDER_DX11_DRIVER_COMMON_HEADER
#define RENDER_DX11_DRIVER_COMMON_HEADER

#include <D3D11.h>

#include <xtl/intrusive_ptr.h>

#include <render/low_level/state_block.h>

namespace render
{

namespace low_level
{

namespace dx11
{

typedef xtl::com_ptr<IDXGIAdapter>             DxAdapterPtr;
typedef xtl::com_ptr<IDXGIFactory>             DxFactoryPtr;
typedef xtl::com_ptr<IDXGIOutput>              DxOutputPtr;
typedef xtl::com_ptr<IDXGISwapChain>           DxSwapChainPtr;
typedef xtl::com_ptr<ID3D11DeviceContext>      DxContextPtr;
typedef xtl::com_ptr<ID3D11Device>             DxDevicePtr;
typedef xtl::com_ptr<ID3D11View>               DxViewPtr;
typedef xtl::com_ptr<ID3D11Resource>           DxResourcePtr;
typedef xtl::com_ptr<ID3D11Texture2D>          DxTexture2DPtr;
typedef xtl::com_ptr<ID3D11SamplerState>       DxSamplerStatePtr;
typedef xtl::com_ptr<ID3D11ShaderResourceView> DxShaderResourceViewPtr;
typedef xtl::com_ptr<ID3D11DeviceChild>        DxDeviceChildPtr;
typedef xtl::com_ptr<ID3D11ComputeShader>      DxComputeShaderPtr;
typedef xtl::com_ptr<ID3D11DomainShader>       DxDomainShaderPtr;
typedef xtl::com_ptr<ID3D11GeometryShader>     DxGeometryShaderPtr;
typedef xtl::com_ptr<ID3D11HullShader>         DxHullShaderPtr;
typedef xtl::com_ptr<ID3D11PixelShader>        DxPixelShaderPtr;
typedef xtl::com_ptr<ID3D11VertexShader>       DxVertexShaderPtr;
typedef xtl::com_ptr<ID3D11InputLayout>        DxInputLayoutPtr;

}

}

}

#endif
