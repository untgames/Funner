#ifndef RENDER_DX11_DRIVER_SWAP_CHAIN_HEADER
#define RENDER_DX11_DRIVER_SWAP_CHAIN_HEADER

#include <render/low_level/driver.h>

#include <shared/object.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISwapChainImpl: virtual public ISwapChain, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IDXGISwapChain& GetHandle () = 0;
};

}

}

}

#endif