#ifndef RENDER_DX11_DRIVER_SOURCES_HEADER
#define RENDER_DX11_DRIVER_SOURCES_HEADER

#include <cmath>

#include <D3D11.h>

#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <common/component.h>
#include <common/strlib.h>
#include <common/utf_converter.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>
#include <render/low_level/utils.h>

#include <render/low_level/helpers/object.h>
#include <render/low_level/helpers/property_list.h>

#include <shared/error.h>

namespace render
{

namespace low_level
{

namespace dx11
{

class Adapter;

using helpers::Object;
using helpers::PropertyList;

typedef xtl::com_ptr<IDXGIAdapter>        DxAdapterPtr;
typedef xtl::com_ptr<IDXGIFactory>        DxFactoryPtr;
typedef xtl::com_ptr<IDXGIOutput>         DxOutputPtr;
typedef xtl::com_ptr<ID3D11DeviceContext> DxContextPtr;
typedef xtl::com_ptr<ID3D11Device>        DxDevicePtr;

typedef xtl::com_ptr<Adapter> AdapterPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Output: virtual public IOutput, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Output  (const DxOutputPtr& output);
    ~Output ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetModesCount ();
    void   GetModeDesc   (size_t mode_index, OutputModeDesc& mode_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �����-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCurrentMode     (const OutputModeDesc&);
    void GetCurrentMode     (OutputModeDesc&);
    void RestoreDefaultMode ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetGammaRamp (const Color3f table [256]);
    void GetGammaRamp (Color3f table [256]);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

  private:
    typedef stl::vector<OutputModeDesc> ModeArray;

  private:
    DxOutputPtr  output;     //���������� ������
    stl::string  name;       //��� ����������
    PropertyList properties; //�������� ����������
    ModeArray    modes;      //������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Adapter: virtual public IAdapter, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Adapter  (const DxFactoryPtr& factory, const char* name, const char* dll_path, const char* init_string);
    Adapter  (const DxAdapterPtr& adapter);
    ~Adapter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� / ���� � ������ / �������� / ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName        ();
    const char* GetPath        ();
    const char* GetDescription ();
    HMODULE     GetModule      ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ������� DX11
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDXGIAdapter& DxAdapter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount      ();
    IOutput* GetOutput            (size_t index);
//    Output*  FindContainingOutput (HWND window);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

  private:
    void Init ();

  private:
    typedef xtl::com_ptr<Output>   OutputPtr;
    typedef stl::vector<OutputPtr> OutputArray;

  private:
    DxAdapterPtr adapter;      //�������          
    stl::string  name;         //��� ��������
    stl::string  path;         //���� � ��������
    PropertyList properties;   //�������� ��������
    OutputArray  outputs;      //���������� ������
    HMODULE      module;       //���������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChain: virtual public ISwapChain, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChain  (const SwapChainDesc& desc);
    ~SwapChain ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* GetAdapter ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (SwapChainDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������ � ������������ �������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IOutput* GetContainingOutput ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ������ ��������� full-screen mode
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFullscreenState (bool state);
    bool GetFullscreenState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������� ������ � ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Present ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();
    
  private:
    PropertyList properties; //�������� ������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Device: virtual public IDevice, public Object
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device  (const AdapterPtr& adapter, const char* init_string);
    ~Device ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        GetCaps                        (DeviceCaps&);
    const char* GetCapString                   (DeviceCapString);
    const char* GetVertexAttributeSemanticName (VertexAttributeSemantic semantic);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IInputLayout*             CreateInputLayout             (const InputLayoutDesc&);
    IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&);
    IRasterizerState*         CreateRasterizerState         (const RasterizerDesc&);
    IBlendState*              CreateBlendState              (const BlendDesc&);
    IDepthStencilState*       CreateDepthStencilState       (const DepthStencilDesc&);
    ISamplerState*            CreateSamplerState            (const SamplerDesc&);
    IBuffer*                  CreateBuffer                  (const BufferDesc&);
    IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    ITexture*                 CreateTexture                 (const TextureDesc&);
    ITexture*                 CreateTexture                 (const TextureDesc&, const TextureData&);
    ITexture*                 CreateTexture                 (const TextureDesc&, IBuffer* buffer, size_t buffer_offset, const size_t* mip_sizes = 0);
    ITexture*                 CreateRenderTargetTexture     (ISwapChain* swap_chain, size_t buffer_index);
    ITexture*                 CreateDepthStencilTexture     (ISwapChain* swap_chain);
    IView*                    CreateView                    (ITexture* texture, const ViewDesc&);
    IPredicate*               CreatePredicate               ();
    IQuery*                   CreateQuery                   (QueryType type);
    IStateBlock*              CreateStateBlock              (const StateBlockMask& mask);
    IDeviceContext*           CreateDeferredContext         ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������������� ��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDeviceContext* GetImmediateContext ();

  private:
    AdapterPtr   adapter;    //������� ����������
    PropertyList properties; //�������� ����������
    DxDevicePtr  device;     //���������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ��� DX11
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetCaps (DriverCaps&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t    GetAdaptersCount ();
    IAdapter* GetAdapter       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IAdapter* CreateAdapter (const char* name, const char* path, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& desc);
    ISwapChain* CreateSwapChain (IDevice* device, const SwapChainDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain* swap_chain, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ���������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetDebugLog (const LogFunction&);
    const LogFunction& GetDebugLog ();

  private:
    void RegisterAdapter   (IAdapter*);
    void UnregisterAdapter (IAdapter*);

  private: 
    typedef stl::vector<IAdapter*>  AdapterWeakArray;
    typedef xtl::com_ptr<Adapter>   AdapterPtr;
    typedef stl::vector<AdapterPtr> AdapterArray;

  private:
    DxFactoryPtr     factory;             //������� DXGI
    AdapterArray     adapters;            //�������� ���������
    AdapterWeakArray registered_adapters; //������������������ �������� ���������
    LogFunction      log_fn;              //������� ����������� ����������������
};

}

}

}

#endif
