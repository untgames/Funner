#include "scene_render_shared.h"

using namespace render;

namespace
{

/*
    ���������
*/

const size_t VIEW_ARRAY_RESERVE  = 8;   //������������� ���������� �������� ���������
const size_t DEFAULT_AREA_WIDTH  = 100; //������ �������� ������������ �� ���������
const size_t DEFAULT_AREA_HEIGHT = 100; //������ �������� ������������ �� ���������

}

/*
    �������� ���������� ���� ����������
*/

typedef xtl::intrusive_ptr<RenderView> ViewPtr;
typedef stl::vector<ViewPtr>               ViewArray;

struct RenderTarget::Impl: private IDesktopListener, private RenderView::IRenderTargetAPI, public xtl::reference_counter, public RenderTargetBase
{
  public:
///�����������
    Impl (RenderTargetManager& manager, const char* in_color_attachment_name, const char* in_depth_stencil_attachment_name) :  
      RenderTargetBase (manager),
      color_attachment_name (in_color_attachment_name),
      depth_stencil_attachment_name (in_depth_stencil_attachment_name),
      desktop (0),
      area (0, 0, DEFAULT_AREA_WIDTH, DEFAULT_AREA_HEIGHT),
      need_update_background_color (true),
      need_update_areas (true),
      need_reorder (true)
    {
      views.reserve (VIEW_ARRAY_RESERVE);    
    }

///����������
    ~Impl ()
    {
      SetDesktop (0);
    }

///����� ��������������� �������
    const char* ColorAttachment () { return color_attachment_name.c_str (); }
    const char* DepthStencilAttachment () { return depth_stencil_attachment_name.c_str (); }
    
///������� ������� ����
    render::Desktop* Desktop () { return desktop; }

///����� �������� �������� �����
    void SetDesktop (render::Desktop* in_desktop)
    {
        //������ ����������� ���������

      if (desktop)
      {
        desktop->DetachListener (this);
        views.clear ();
      }
        
        //��������� �������� �������� �����

      desktop = in_desktop;

        //����������� ���������

      if (desktop)
      {
        for (size_t i=0, count=desktop->ViewportsCount (); i<count; i++)
          OnAttachViewport (desktop->Viewport (i));

        desktop->AttachListener (this);
      }
    }
    
///������� ���������� ���� ������
    const Rect& Area () { return area; }
    
///��������� ����������� ���� ������
    void SetArea (const Rect& rect)
    {
      if (area.left == rect.left && area.top == rect.top && area.width == rect.width && area.height == rect.height)
        return;      

      area = rect;

      need_update_areas = true;
    }
    
///�����������
    void Draw ()
    {
      static const char* METHOD_NAME = "render::RenderTarget::Impl::Draw";
      
      RenderTargetManager&  manager             = Manager ();
      RenderPathManager*    render_path_manager = manager.RenderPathManager ();
      mid_level::IRenderer* renderer            = render_path_manager ? render_path_manager->Renderer () : 0;

      if (!renderer)
        throw xtl::format_operation_exception (METHOD_NAME, "Null renderer");

      if (!manager.BeginDraw ())
        return;

      try
      {      
          //������������ �������� ������
        
        if (need_reorder)
          SortViews ();
          
          //���������� ������ �������� ������

        if (need_update_areas)
          UpdateAreas ();
         
          //������� ���� ����������
          
        ClearRenderTarget (*renderer);

          //��������� �������� ������

        for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        {
          try
          {
            (*iter)->Draw ();
          }
          catch (xtl::exception& exception)
          {
            manager.EndDraw ();

            Viewport& viewport = (*iter)->Viewport ();

            exception.touch ("draw viewport='%s', render_path='%s'", viewport.Name (), viewport.RenderPath ());          

            throw;
          }
        }        
      }
      catch (xtl::exception& exception)
      {
        manager.EndDraw ();
        
        exception.touch (METHOD_NAME);
        
        throw;
      }
      catch (...)
      {              
        manager.EndDraw ();
        throw;
      }

      manager.EndDraw ();
    }
    
  private:
///������� ���� ����������
    void ClearRenderTarget (mid_level::IRenderer& renderer)
    {
      try
      {      
        if (!clear_frame)
        {
            //�������� ���������� �����

          clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);

          render::mid_level::IRenderTarget *render_target        = renderer.GetColorBuffer (),
                                           *depth_stencil_target = renderer.GetDepthStencilBuffer ();

          clear_frame->SetRenderTargets (render_target, depth_stencil_target);
          clear_frame->SetFlags         (render::mid_level::ClearFlag_All);
        }
        
          //���������� ����� �������
          
        if (need_update_background_color && desktop)
        {
          clear_frame->SetColor (desktop->BackgroundColor ());

          need_update_background_color = false;
        }

          //�������

        renderer.AddFrame (clear_frame.get ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderTarget::Impl::ClearRenderTarget");
        throw;
      }
    }
  
///���������� ������ �������� ������
    void UpdateAreas ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->UpdateAreaNotify ();
        
      need_update_areas = false;
    }

///���������� �������� ������
    void SortViews ()
    {
      struct ViewComparator
      {
        bool operator () (const ViewPtr& vp1, const ViewPtr& vp2) const { return vp1->Viewport ().ZOrder () < vp2->Viewport ().ZOrder (); }
      };

      stl::sort (views.begin (), views.end (), ViewComparator ());
      
      need_reorder = false;    
    }  
    
///��������� ������ ������� ����������    
    const Rect& GetRenderTargetArea  () { return area; }
    
///��������� ��������� ����� ����������
    RenderPathManager* GetRenderPathManager () { return Manager ().RenderPathManager (); }

///���������� �� ���������� ������� ���������� �������� ������    
    void UpdateOrderNotify ()
    {
      need_reorder = true;
    }
  
///���������� �� ��������� ����� ����
    void OnChangeBackgroundColor (const math::vec4f&)
    {
      need_update_background_color = true;
    }

///���������� � ���������� ������� ������
    void OnAttachViewport (Viewport& viewport)
    {
      views.push_back (ViewPtr (new RenderView (viewport, *this), false));
    }

///���������� �� �������� ������� ������
    void OnDetachViewport (Viewport& viewport)
    {
      size_t id = viewport.Id ();

      for (ViewArray::iterator iter=views.begin (); iter!=views.end ();)
        if ((*iter)->Viewport ().Id () == id) views.erase (iter);
        else                                  ++iter;
    }

///���������� �� �������� �������� �����
    void OnDestroy ()
    {
      SetDesktop (0);
    }

///����� ��������
    void FlushResources ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->FlushResources ();
        
      clear_frame = 0;
    }

///������� ����� ������ (�� ���������, ����������� ��������� ����� �������� ��� �������� Impl ��� �������� �������� �����)
    void AddRef () {}
    void Release () {}
    
  private:
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;

  private:
    stl::string      color_attachment_name;         //��� ���������������� ������ �����
    stl::string      depth_stencil_attachment_name; //��� ���������������� ������ ������������� ���������
    render::Desktop* desktop;                       //������� ����
    Rect             area;                          //���������� ���� ������
    ViewArray        views;                         //������ �������� ����������
    ClearFramePtr    clear_frame;                   //��������� ����
    bool             need_update_background_color;  //���������� �������� ���� ����    
    bool             need_update_areas;             //���������� �������� ������� �������� ������
    bool             need_reorder;                  //���������� ��������������� ������� ������
};

/*
    ������������ / ���������� / ������������
*/

RenderTarget::RenderTarget (RenderTargetManager& manager, const char* color_attachment_name, const char* depth_stencil_attachment_name)
{
  static const char* METHOD_NAME = "render::RenderTarget";

  if (!color_attachment_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "color_attachment_name");
    
  if (!depth_stencil_attachment_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "depth_stencil_attachment_name");

  impl = new Impl (manager, color_attachment_name, depth_stencil_attachment_name);
}

RenderTarget::RenderTarget (const RenderTarget& target)
  : impl (target.impl)
{
  addref (impl);
}

RenderTarget::~RenderTarget ()
{
  release (impl);
}

RenderTarget& RenderTarget::operator = (const RenderTarget& target)
{
  RenderTarget (target).Swap (*this);
  
  return *this;
}

/*
    ������������� ���� ����������
*/

size_t RenderTarget::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ����� ��������������� �������
*/

const char* RenderTarget::Attachment (RenderTargetAttachment buffer_id) const
{
  switch (buffer_id)
  {
    case RenderTargetAttachment_Color:        return impl->ColorAttachment ();
    case RenderTargetAttachment_DepthStencil: return impl->DepthStencilAttachment ();
    default:
      throw xtl::make_argument_exception ("render::RenderTarget::Attachment", "buffer_id");
  }
}

/*
    ���������� ������� �������� ������ ����������
*/

void RenderTarget::SetArea (const Rect& rect)
{
  impl->SetArea (rect);
}

void RenderTarget::SetArea (int left, int top, size_t width, size_t height)
{
  SetArea (Rect (left, top, width, height));
}

void RenderTarget::SetOrigin (int left, int top)
{
  const Rect& area = impl->Area ();

  SetArea (Rect (left, top, area.width, area.height));
}

void RenderTarget::SetSize (size_t width, size_t height)
{
  const Rect& area = impl->Area ();

  SetArea (Rect (area.left, area.top, width, height));
}

const Rect& RenderTarget::Area () const
{
  return impl->Area ();
}

/*
    ������� ���� (�������� �������� - weak-ref)
*/

void RenderTarget::SetDesktop (render::Desktop* desktop)
{
  impl->SetDesktop (desktop);
}

Desktop* RenderTarget::Desktop ()
{
  return impl->Desktop ();
}

const Desktop* RenderTarget::Desktop () const
{
  return impl->Desktop ();
}

/*
    ����������
*/

void RenderTarget::Update ()
{
  impl->Draw ();
}

/*
    ������ ����������� (screen-shot)
*/

void RenderTarget::CaptureImage (media::Image&)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::CaptureImage(media::Image&)");
}

void RenderTarget::CaptureImage (const char* image_name)
{
  throw xtl::make_not_implemented_exception ("render::SceneRender::CaptureImage(const char*)");
}

/*
    �����
*/

void RenderTarget::Swap (RenderTarget& target)
{
  stl::swap (impl, target.impl);
}

namespace render
{

void swap (RenderTarget& target1, RenderTarget& target2)
{
  target1.Swap (target2);
}

}
