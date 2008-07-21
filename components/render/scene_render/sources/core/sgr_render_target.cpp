#include "scene_render_shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const size_t VIEW_ARRAY_RESERVE  = 8;   //резервируемое количество областей видимости
const size_t DEFAULT_AREA_WIDTH  = 100; //ширина рабочего пространства по умолчанию
const size_t DEFAULT_AREA_HEIGHT = 100; //высота рабочего пространства по умолчанию

}

namespace render
{

/*
    Базовый класс реализации цели рендеринга
*/

class IRenderTargetImpl: public xtl::reference_counter
{
  public:  
    virtual ~IRenderTargetImpl () {}

///Имена ассоциированных буферов
    virtual const char* ColorAttachment () = 0;
    virtual const char* DepthStencilAttachment () = 0;

///Текущий рабочий стол
    virtual render::Desktop* Desktop () = 0;

///Смена текущего рабочего стола
    virtual void SetDesktop (render::Desktop* in_desktop) = 0;
    
///Текущее логическое окно вывода
    virtual const Rect& DesktopArea () = 0;
    
///Изменение логического окна вывода
    virtual void SetDesktopArea (const Rect& rect) = 0;
    
///Текущее физическое окно вывода
    virtual const Rect& RenderableArea () = 0;
    
///Изменение физическое окна вывода
    virtual void SetRenderableArea (const Rect& rect) = 0;
    
///Перерисовка
    virtual void Draw () = 0;
};

}

/*
    Пустая цель рендеринга    
*/

namespace
{

class NullRenderTarget: public IRenderTargetImpl
{
  public:
///Имена ассоциированных буферов
    const char* ColorAttachment () { return ""; }
    const char* DepthStencilAttachment () { return ""; }

///Текущий рабочий стол
    render::Desktop* Desktop () { return 0; }

///Смена текущего рабочего стола
    void SetDesktop (render::Desktop*)
    {
      Raise ("render::NullRenderTarget::SetDesktop");
    }
    
///Текущее логическое окно вывода
    const Rect& DesktopArea () { return desktop_area; }
    
///Изменение логического окна вывода
    void SetDesktopArea (const Rect&)
    {
      Raise ("render::NullRenderTarget::SetDesktopArea");
    }
    
///Текущее физическое окно вывода
    const Rect& RenderableArea () { return renderable_area; }
    
///Изменение физическое окна вывода
    void SetRenderableArea (const Rect&)
    {
      Raise ("render::NullRenderTarget::SetRenderableArea");
    }

///Перерисовка
    void Draw ()
    {
      Raise ("render::NullRenderTarget::Draw");
    }

///Получение экземпляра
    static NullRenderTarget* Instance ()
    {
      static char              buffer [sizeof NullRenderTarget];
      static NullRenderTarget* target = new (buffer) NullRenderTarget;

      return target;
    }

  private:
    NullRenderTarget () {}
  
    void Raise (const char* source)
    {
      throw xtl::format_operation_exception (source, "Invalid operation on null render-target");      
    }
    
  private:
    Rect desktop_area;
    Rect renderable_area;
};

/*
    Описание реализации цели рендеринга
*/

class RenderTargetImpl: private IDesktopListener, private RenderView::IRenderTargetAPI, public IRenderTargetImpl, public RenderTargetBase
{
  public:
///Конструктор
    RenderTargetImpl (RenderTargetManager& manager, const char* in_color_attachment_name, const char* in_depth_stencil_attachment_name) :  
      RenderTargetBase (manager),
      color_attachment_name (in_color_attachment_name),
      depth_stencil_attachment_name (in_depth_stencil_attachment_name),
      desktop (0),
      desktop_area (0, 0, DEFAULT_AREA_WIDTH, DEFAULT_AREA_HEIGHT),
      need_update_background_color (true),
      need_update_areas (true),
      need_reorder (true)
    {
        //резервирование памяти для хранения областей вывода

      views.reserve (VIEW_ARRAY_RESERVE);
      
        //инициализация физически визуализируемой области рендеринга

      RenderPathManager*                render_path_manager = manager.RenderPathManager ();
      mid_level::IRenderer*             renderer            = render_path_manager ? render_path_manager->Renderer () : 0;
      render::mid_level::IRenderTarget* render_target       = renderer->GetColorBuffer (); //заменить в будущем на attachment!!!
      
      if (render_target)
      {
        renderable_area.width  = render_target->GetWidth ();
        renderable_area.height = render_target->GetHeight ();
      }
    }

///Деструктор
    ~RenderTargetImpl ()
    {
      SetDesktop (0);
    }

///Имена ассоциированных буферов
    const char* ColorAttachment () { return color_attachment_name.c_str (); }
    const char* DepthStencilAttachment () { return depth_stencil_attachment_name.c_str (); }
    
///Текущий рабочий стол
    render::Desktop* Desktop () { return desktop; }

///Смена текущего рабочего стола
    void SetDesktop (render::Desktop* in_desktop)
    {
        //отмена регистрации слушателя

      if (desktop)
      {
        desktop->DetachListener (this);
        views.clear ();
      }
        
        //изменение текущего рабочего стола

      desktop = in_desktop;

        //регистрация слушателя

      if (desktop)
      {
        for (size_t i=0, count=desktop->ViewportsCount (); i<count; i++)
          OnAttachViewport (desktop->Viewport (i));

        desktop->AttachListener (this);
      }
    }
    
///Текущее логическое окно вывода
    const Rect& DesktopArea () { return desktop_area; }
    
///Изменение логического окна вывода
    void SetDesktopArea (const Rect& rect)
    {
      if (desktop_area.left == rect.left && desktop_area.top == rect.top && desktop_area.width == rect.width && desktop_area.height == rect.height)
        return;      

      desktop_area = rect;

      need_update_areas = true;
    }
    
///Текущее физическое окно вывода
    const Rect& RenderableArea () { return renderable_area; }
    
///Изменение физическое окна вывода
    void SetRenderableArea (const Rect& rect)
    {
      if (renderable_area.left == rect.left && renderable_area.top == rect.top && renderable_area.width == rect.width && renderable_area.height == rect.height)
        return;      

      renderable_area = rect;

      need_update_areas = true;
    }    
    
///Перерисовка
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
          //упорядочение областей вывода
        
        if (need_reorder)
          SortViews ();
          
          //обновление границ областей вывода

        if (need_update_areas)
          UpdateAreas ();
         
          //очистка цели рендеринга
          
        ClearRenderTarget (*renderer);

          //рисование областей вывода

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
///Очистка цели рендеринга
    void ClearRenderTarget (mid_level::IRenderer& renderer)
    {
      try
      {      
        if (!clear_frame)
        {
            //создание очищающего кадра

          clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);

          render::mid_level::IRenderTarget *render_target        = renderer.GetColorBuffer (),
                                           *depth_stencil_target = renderer.GetDepthStencilBuffer ();

          clear_frame->SetRenderTargets (render_target, depth_stencil_target);
          clear_frame->SetFlags         (render::mid_level::ClearFlag_All);
        }
        
          //обновление цвета очистки
          
        if (need_update_background_color && desktop)
        {
          clear_frame->SetColor (desktop->BackgroundColor ());

          need_update_background_color = false;
        }

          //очистка

        renderer.AddFrame (clear_frame.get ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderTarget::Impl::ClearRenderTarget");
        throw;
      }
    }
  
///Обновление границ областей вывода
    void UpdateAreas ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->UpdateAreaNotify ();

      need_update_areas = false;
    }

///Сортировка областей вывода
    void SortViews ()
    {
      struct ViewComparator
      {
        bool operator () (const ViewPtr& vp1, const ViewPtr& vp2) const { return vp1->Viewport ().ZOrder () < vp2->Viewport ().ZOrder (); }
      };

      stl::sort (views.begin (), views.end (), ViewComparator ());
      
      need_reorder = false;    
    }  
    
///Получение границ области рендеринга
    const Rect& GetRenderableArea  () { return renderable_area; }
    const Rect& GetDesktopArea () { return desktop_area; }
    
///Получение менеджера путей рендеринга
    RenderPathManager* GetRenderPathManager () { return Manager ().RenderPathManager (); }

///Оповещение об обновлении порядка следования областей вывода    
    void UpdateOrderNotify ()
    {
      need_reorder = true;
    }
  
///Оповещение об изменении цвета фона
    void OnChangeBackgroundColor (const math::vec4f&)
    {
      need_update_background_color = true;
    }

///Оповещение о добавлении области вывода
    void OnAttachViewport (Viewport& viewport)
    {
      views.push_back (ViewPtr (new RenderView (viewport, *this), false));
    }

///Оповещение об удалении области вывода
    void OnDetachViewport (Viewport& viewport)
    {
      size_t id = viewport.Id ();

      for (ViewArray::iterator iter=views.begin (); iter!=views.end ();)
        if ((*iter)->Viewport ().Id () == id) views.erase (iter);
        else                                  ++iter;
    }

///Оповещение об удалении рабочего стола
    void OnDestroy ()
    {
      SetDesktop (0);
    }

///Сброс ресурсов
    void FlushResources ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->FlushResources ();
        
      clear_frame = 0;
    }

///Подсчёт числа ссылок (не требуется, регистрация слушателя будет отменена при удалении Impl или текущего рабочего стола)
    void AddRef () {}
    void Release () {}
    
  private:
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;
    typedef xtl::intrusive_ptr<RenderView>               ViewPtr;
    typedef stl::vector<ViewPtr>                         ViewArray;

  private:
    stl::string      color_attachment_name;         //имя ассоциированного буфера цвета
    stl::string      depth_stencil_attachment_name; //имя ассоциированного буфера попиксельного отсечения
    render::Desktop* desktop;                       //рабочий стол
    Rect             desktop_area;                  //логическое окно вывода
    Rect             renderable_area;               //физическое окно вывода
    ViewArray        views;                         //массив областей рендеринга
    ClearFramePtr    clear_frame;                   //очищающий кадр
    bool             need_update_background_color;  //необходимо обновить цвет фона    
    bool             need_update_areas;             //необходимо обновить границы областей вывода
    bool             need_reorder;                  //необходимо пересортировать области вывода
};

}

/*
    Конструкторы / деструктор / присваивание
*/

RenderTarget::RenderTarget ()
  : impl (NullRenderTarget::Instance ())
{
  addref (impl);
}

RenderTarget::RenderTarget (RenderTargetManager& manager, const char* color_attachment_name, const char* depth_stencil_attachment_name)
{
  static const char* METHOD_NAME = "render::RenderTarget";

  if (!color_attachment_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "color_attachment_name");
    
  if (!depth_stencil_attachment_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "depth_stencil_attachment_name");

  impl = new RenderTargetImpl (manager, color_attachment_name, depth_stencil_attachment_name);
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
    Идентификатор цели рендеринга
*/

size_t RenderTarget::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    Имена ассоциированных буферов
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
    Границы логически отображаемой области рабочего стола
*/

void RenderTarget::SetDesktopArea (const Rect& rect)
{
  try
  {
    impl->SetDesktopArea (rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetDesktopArea");
    throw;
  }
}

void RenderTarget::SetDesktopArea (int left, int top, size_t width, size_t height)
{
  SetDesktopArea (Rect (left, top, width, height));
}

const Rect& RenderTarget::DesktopArea () const
{
  return impl->DesktopArea ();
}

/*
    Физические границы визуализируемой области (например, в оконных координатах)
*/

void RenderTarget::SetRenderableArea (const Rect& rect)
{
  try
  {
    impl->SetRenderableArea (rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetRenderableArea");
    throw;
  }
}

void RenderTarget::SetRenderableArea (int left, int top, size_t width, size_t height)
{
  SetRenderableArea (Rect (left, top, width, height));
}

const Rect& RenderTarget::RenderableArea () const
{
  return impl->RenderableArea ();
}

/*
    Рабочий стол (политика владения - weak-ref)
*/

void RenderTarget::SetDesktop (render::Desktop* desktop)
{
  try
  {
    impl->SetDesktop (desktop);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetDesktop");
    throw;
  }
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
    Обновление
*/

void RenderTarget::Update ()
{
  try
  {
    impl->Draw ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::Update");
    throw;
  }  
}

/*
    Захват изображения (screen-shot)
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
    Обмен
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
