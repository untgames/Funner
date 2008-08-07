#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_BASIC_RENDERER_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_BASIC_RENDERER_HEADER

#include <stl/vector>

#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

#include <render/mid_level/driver.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //'class1' : inherits 'class2::member' via dominance
#endif

namespace render
{

namespace mid_level
{

namespace low_level_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Object: virtual public IObject, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();    
    void Release ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderTargetType
{
  RenderTargetType_Color,        //����� �����
  RenderTargetType_DepthStencil, //����� ������������� ���������
  
  RenderTargetType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTarget: virtual public IRenderTarget, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget (render::low_level::IView* render_target_view, RenderTargetType type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetType GetType () { return type; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetWidth  ();
    size_t GetHeight ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� View
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::IView* GetView () { return render_target_view.get (); } 
  
  private:
    typedef xtl::com_ptr<render::low_level::IView> ViewPtr;

  private:
    ViewPtr          render_target_view;
    RenderTargetType type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicFrame: virtual public IFrame, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetRenderTargets      (IRenderTarget* render_target, IRenderTarget* depth_stencil_target);
    IRenderTarget* GetRenderTarget       (); //������� ����� �����
    IRenderTarget* GetDepthStencilTarget (); //������� ����� ������������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetViewport (const render::mid_level::Viewport&);
    void GetViewport (render::mid_level::Viewport&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw (render::low_level::IDevice* device);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindViewport (render::low_level::IDevice* device);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void DrawCore (render::low_level::IDevice* device) = 0;

  private:
    typedef xtl::com_ptr<RenderTarget> RenderTargetPtr;

  private:
    RenderTargetPtr             render_target;
    RenderTargetPtr             depth_stencil_target;
    render::low_level::Viewport viewport;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicRenderer: virtual public IRenderer, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicRenderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ���� ��������
///  (��������� ��� ����������� ������������� ��������, ��������� �� ������ IRenderer)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetResourcePoolId ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����� � ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderTarget* GetColorBuffer ();
    IRenderTarget* GetDepthStencilBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderTarget* CreateDepthStencilBuffer ();
    IRenderTarget* CreateRenderBuffer       ();
    IClearFrame*   CreateClearFrame         ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddFrame (IFrame*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� / ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawFrames   ();
    void CancelFrames ();    
    
  private:
    typedef xtl::com_ptr<IRenderTarget>                 RenderTargetPtr;
    typedef xtl::com_ptr<render::low_level::IDevice>    DevicePtr;
    typedef xtl::com_ptr<render::low_level::ISwapChain> SwapChainPtr;
    typedef xtl::com_ptr<BasicFrame>                    FramePtr;
    typedef stl::vector<FramePtr>                       FrameArray;

  protected:
    DevicePtr device;

  private:
    RenderTargetPtr color_buffer;
    RenderTargetPtr depth_stencil_buffer;
    SwapChainPtr    swap_chain;
    FrameArray      frames;
    size_t          resource_pool_id;
};

}

}

}

#endif
