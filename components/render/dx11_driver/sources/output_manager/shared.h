#ifndef RENDER_DX11_DRIVER_OUTPUT_STAGE_SHARED_HEADER
#define RENDER_DX11_DRIVER_OUTPUT_STAGE_SHARED_HEADER

#include <render/low_level/utils.h>

#include <shared/error.h>
#include <shared/output_manager.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class BlendState: virtual public IBlendState, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BlendState  (const DeviceManager&, const BlendDesc&);
    ~BlendState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (BlendDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11BlendState& GetHandle () { return *state; }

  private:
    BlendDesc       desc;  //���������� ���������
    DxBlendStatePtr state; //���������
};

typedef xtl::com_ptr<BlendState> BlendStatePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilState: virtual public IDepthStencilState, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DepthStencilState  (const DeviceManager&, const DepthStencilDesc&);
    ~DepthStencilState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (DepthStencilDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11DepthStencilState& GetHandle () { return *state; }

  private:
    DepthStencilDesc       desc;  //���������� ���������
    DxDepthStencilStatePtr state; //���������
};

typedef xtl::com_ptr<DepthStencilState> DepthStencilStatePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RasterizerState : virtual public IRasterizerState, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizerState  (const DeviceManager& manager, const RasterizerDesc& desc);
    ~RasterizerState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (RasterizerDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11RasterizerState& GetHandle () { return *state; }

  private:
    RasterizerDesc       desc;   //���������� ���������
    DxRasterizerStatePtr state;  //���������
};

typedef xtl::com_ptr<RasterizerState> RasterizerStatePtr;

}

}

}

#endif