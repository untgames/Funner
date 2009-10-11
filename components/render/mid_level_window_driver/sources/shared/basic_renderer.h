#ifndef RENDER_MID_WINDOW_DRIVER_BASIC_RENDERER_HEADER
#define RENDER_MID_WINDOW_DRIVER_BASIC_RENDERER_HEADER

#include <stl/auto_ptr.h>
#include <stl/vector>
#include <stl/list>

#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>
#include <xtl/trackable_ptr.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

#include <render/mid_level/driver.h>
#include <render/mid_level/renderer2d.h>
#include <render/mid_level/low_level_renderer.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //'class1' : inherits 'class2::member' via dominance
#endif

namespace render
{

namespace mid_level
{

namespace window_driver
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
class RenderTarget: virtual public IRenderTarget, public Object, public xtl::trackable
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
    size_t GetWidth  () { return width; }
    size_t GetHeight () { return height; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� View
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::IView* GetView () { return render_target_view.get (); } 
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetSize (size_t width, size_t height);

  private:
    typedef xtl::com_ptr<render::low_level::IView> ViewPtr;

  private:
    ViewPtr          render_target_view;
    RenderTargetType type;
    size_t           width;
    size_t           height;
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
    void Draw (render::low_level::IDevice* device, size_t& draw_frames_count);

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
    typedef xtl::trackable_ptr<RenderTarget> RenderTargetPtr;

  private:
    RenderTargetPtr             render_target;
    RenderTargetPtr             depth_stencil_target;
    render::low_level::Viewport viewport;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� 2D-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Renderer2D
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Renderer2D (low_level::IDevice* device);
    ~Renderer2D ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::renderer2d::ITexture*   CreateTexture   (const media::Image& image);
    mid_level::renderer2d::ITexture*   CreateTexture   (const media::CompressedImage& image);
    mid_level::renderer2d::ITexture*   CreateTexture   (size_t width, size_t height, PixelFormat pixel_format);
    mid_level::renderer2d::IPrimitive* CreatePrimitive ();
    mid_level::renderer2d::IFrame*     CreateFrame     ();

  private:
    Renderer2D (const Renderer2D&);             //no impl
    Renderer2D& operator = (const Renderer2D&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LowLevelRenderer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LowLevelRenderer  (low_level::IDevice* device);
    ~LowLevelRenderer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::ILowLevelFrame* CreateFrame ();

  private:
    LowLevelRenderer (const LowLevelRenderer&);             //no impl
    LowLevelRenderer& operator = (const LowLevelRenderer&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
