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
///Состояние подуровня смешивания цветов
///////////////////////////////////////////////////////////////////////////////////////////////////
class BlendState: virtual public IBlendState, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    BlendState  (const DeviceManager&, const BlendDesc&);
    ~BlendState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (BlendDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкроуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11BlendState& GetHandle () { return *state; }

  private:
    BlendDesc       desc;  //дескриптор состояния
    DxBlendStatePtr state; //состояние
};

typedef xtl::com_ptr<BlendState> BlendStatePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние подуровня попиксельного отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilState: virtual public IDepthStencilState, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DepthStencilState  (const DeviceManager&, const DepthStencilDesc&);
    ~DepthStencilState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (DepthStencilDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкроуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11DepthStencilState& GetHandle () { return *state; }

  private:
    DepthStencilDesc       desc;  //дескриптор состояния
    DxDepthStencilStatePtr state; //состояние
};

typedef xtl::com_ptr<DepthStencilState> DepthStencilStatePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние подуровня растеризации
///////////////////////////////////////////////////////////////////////////////////////////////////
class RasterizerState : virtual public IRasterizerState, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizerState  (const DeviceManager& manager, const RasterizerDesc& desc);
    ~RasterizerState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (RasterizerDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкроуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ID3D11RasterizerState& GetHandle () { return *state; }

  private:
    RasterizerDesc       desc;   //дескриптор состояния
    DxRasterizerStatePtr state;  //состояние
};

typedef xtl::com_ptr<RasterizerState> RasterizerStatePtr;

}

}

}

#endif
