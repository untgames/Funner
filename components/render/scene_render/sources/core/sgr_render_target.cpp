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

///Текущий экран
    virtual render::Screen* Screen () = 0;

///Смена текущего экрана
    virtual void SetScreen (render::Screen* in_screen) = 0;
    
///Текущее логическое окно вывода
    virtual const Rect& ScreenArea () = 0;
    
///Изменение логического окна вывода
    virtual void SetScreenArea (const Rect& rect) = 0;
    
///Текущее физическое окно вывода
    virtual const Rect& RenderableArea () = 0;
    
///Изменение физическое окна вывода
    virtual void SetRenderableArea (const Rect& rect) = 0;
    
///Перерисовка
    virtual void Draw () = 0;
    
///Захват изображения
    virtual void CaptureImage (media::Image&) = 0;
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

///Текущий экран
    render::Screen* Screen () { return 0; }

///Смена текущего экрана
    void SetScreen (render::Screen*)
    {
      Raise ("render::NullRenderTarget::SetScreen");
    }
    
///Текущее логическое окно вывода
    const Rect& ScreenArea () { return screen_area; }
    
///Изменение логического окна вывода
    void SetScreenArea (const Rect&)
    {
      Raise ("render::NullRenderTarget::SetScreenArea");
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
    
///Захват изображения
    void CaptureImage (media::Image&)
    {
      Raise ("render::NullRenderTarget::CaptureImage");
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
    Rect screen_area;
    Rect renderable_area;
};

/*
    Описание реализации цели рендеринга
*/

class RenderTargetImpl: private IScreenListener, private RenderView::IRenderTargetAPI, public IRenderTargetImpl, public RenderTargetBase
{
  public:
///Конструктор
    RenderTargetImpl (RenderTargetManager& manager, const char* in_color_attachment_name, const char* in_depth_stencil_attachment_name) :  
      RenderTargetBase (manager),
      color_attachment_name (in_color_attachment_name),
      depth_stencil_attachment_name (in_depth_stencil_attachment_name),
      screen (0),
      screen_area (0, 0, DEFAULT_AREA_WIDTH, DEFAULT_AREA_HEIGHT),
      need_update_attachments (true),
      need_update_background (true),
      need_update_areas (true),
      need_reorder (true)
    {
        //резервирование памяти для хранения областей вывода

      views.reserve (VIEW_ARRAY_RESERVE);      
    }

///Деструктор
    ~RenderTargetImpl ()
    {
      SetScreen (0);
    }

///Имена ассоциированных буферов
    const char* ColorAttachment () { return color_attachment_name.c_str (); }
    const char* DepthStencilAttachment () { return depth_stencil_attachment_name.c_str (); }
    
///Текущий экран
    render::Screen* Screen () { return screen; }

///Смена текущего экрана
    void SetScreen (render::Screen* in_screen)
    {
        //отмена регистрации слушателя

      if (screen)
      {
        screen->DetachListener (this);
        views.clear ();
      }
        
        //изменение текущего экрана

      screen = in_screen;

        //регистрация слушателя

      if (screen)
      {
        for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
          OnAttachViewport (screen->Viewport (i));

        screen->AttachListener (this);
      }
    }
    
///Текущее логическое окно вывода
    const Rect& ScreenArea () { return screen_area; }
    
///Изменение логического окна вывода
    void SetScreenArea (const Rect& rect)
    {
      if (screen_area.left == rect.left && screen_area.top == rect.top && screen_area.width == rect.width && screen_area.height == rect.height)
        return;      

      screen_area = rect;

      need_update_areas = true;
    }
    
///Текущее физическое окно вывода
    const Rect& RenderableArea ()
    {
      if (need_update_attachments)
        UpdateAttachments ();
      
      return renderable_area;
    }
    
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
      
      if (!screen)
        return;

      if (!Manager ().BeginDraw ())
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
        
        ClearRenderTarget ();
        
          //рисование областей вывода

        for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        {
          Viewport& viewport = (*iter)->Viewport ();        
          
          try
          {
            (*iter)->Draw ();
          }
          catch (std::exception& exception)
          {
            LogPrintf ("%s\n    at draw viewport='%s', render_path='%s'", exception.what (), viewport.Name (), viewport.RenderPath ());
          }
          catch (...)
          {
            LogPrintf ("Unknown exception\n    at draw viewport='%s', render_path='%s'", viewport.Name (), viewport.RenderPath ());
          }
        }
      }
      catch (std::exception& exception)
      {
        LogPrintf ("%s\n    at RenderTargetImpl::Draw", exception.what ());
      }
      catch (...)
      {
        LogPrintf ("Unknown exception\n    at RenderTargetImpl::Draw");
      }        

        //завершение транзакции отрисовки

      Manager ().EndDraw ();
    }    

///Захват изображения
    void CaptureImage (media::Image& image)
    {      
      mid_level::IRenderTarget *render_target = 0, *depth_stencil_target = 0;
      
      RenderTargetImpl::GetRenderTargets (render_target, depth_stencil_target);
      
      if (!render_target)
        throw xtl::format_operation_exception ("render::RenderTargetImpl::CaptureImage", "Null render-target buffer");
        
      render_target->CaptureImage (image);
    }

  private:
///Очистка цели рендеринга
    void ClearRenderTarget ()
    {
      try
      {
        if (!need_update_background && !clear_frame)
          return;

        mid_level::IRenderer& renderer = Manager ().Renderer ();
        
        if (need_update_background)
        {
          if (!screen->BackgroundState ())
          {
              //удаление очищающего кадра

            clear_frame            = 0;
            need_update_background = false;

            return;
          }
          
          if (!clear_frame)
          {
              //создание очищающего кадра

            clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);
            
            mid_level::IRenderTarget *render_target = 0, *depth_stencil_target = 0;

            RenderTargetImpl::GetRenderTargets (render_target, depth_stencil_target);
            clear_frame->SetRenderTargets      (render_target, depth_stencil_target);
            clear_frame->SetFlags              (render::mid_level::ClearFlag_All);
          }

            //обновление цвета очистки

          clear_frame->SetColor (screen->BackgroundColor ());

          need_update_background = false;
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
    
///Обновление целевых буферов
    void UpdateAttachments ()
    {
      try
      {
        AttachmentPtr new_color_attachment (Manager ().GetAttachment (("Color." + color_attachment_name).c_str ())),
                      new_depth_stencil_attachment (Manager ().GetAttachment (("DepthStencil." + depth_stencil_attachment_name).c_str ()));
                      
          //инициализация физически визуализируемой области рендеринга
          
        mid_level::IRenderTarget* render_target = new_color_attachment ? new_color_attachment.get () : depth_stencil_attachment.get ();

        if (render_target)
        {
          renderable_area.width  = render_target->GetWidth ();
          renderable_area.height = render_target->GetHeight ();
        }                      
        else
        {
          renderable_area = Rect ();
        }

        color_attachment         = new_color_attachment;
        depth_stencil_attachment = new_depth_stencil_attachment;                

        need_update_attachments = false;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderTargetImpl::UpdateAttachments");
        throw;
      }
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
    
///Получение системы рендеринга
    render::mid_level::IRenderer& GetRenderer () { return Manager ().Renderer (); }
    
///Получение границ области рендеринга
    const Rect& GetRenderableArea  () { return renderable_area; }
    const Rect& GetScreenArea () { return screen_area; }
    
///Получение пути рендеринга
    ICustomSceneRender& GetRenderPath (const char* name) { return Manager ().GetRenderPath (name); }
    
///Получение ассоциированных буферов рендеринга
    void GetRenderTargets (mid_level::IRenderTarget*& render_target, mid_level::IRenderTarget*& depth_stencil_target)
    {
      if (need_update_attachments)
        UpdateAttachments ();      

      render_target        = color_attachment.get ();
      depth_stencil_target = depth_stencil_attachment.get ();
    }

///Оповещение об обновлении порядка следования областей вывода    
    void UpdateOrderNotify ()
    {
      need_reorder = true;
    }
  
///Оповещение об изменении фона
    void OnChangeBackground (bool, const math::vec4f&)
    {
      need_update_background = true;
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

///Оповещение об удалении экрана
    void OnDestroy ()
    {
      SetScreen (0);
    }

///Сброс ресурсов
    void FlushResources ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->FlushResources ();
        
      clear_frame              = 0;
      color_attachment         = 0;
      depth_stencil_attachment = 0;
      need_update_attachments  = true;
      need_update_background   = true;
    }

///Подсчёт числа ссылок (не требуется, регистрация слушателя будет отменена при удалении Impl или текущего экрана)
    void AddRef () {}
    void Release () {}
    
  private:
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;
    typedef xtl::intrusive_ptr<RenderView>               ViewPtr;
    typedef stl::vector<ViewPtr>                         ViewArray;
    typedef xtl::com_ptr<mid_level::IRenderTarget>       AttachmentPtr;

  private:
    stl::string      color_attachment_name;         //имя ассоциированного буфера цвета
    stl::string      depth_stencil_attachment_name; //имя ассоциированного буфера попиксельного отсечения
    AttachmentPtr    color_attachment;              //ассоциированный буфер цвета
    AttachmentPtr    depth_stencil_attachment;      //ассоциированный буфер попиксельного отсечения
    render::Screen*  screen;                        //экран
    Rect             screen_area;                   //логическое окно вывода
    Rect             renderable_area;               //физическое окно вывода
    ViewArray        views;                         //массив областей рендеринга
    ClearFramePtr    clear_frame;                   //очищающий кадр
    bool             need_update_attachments;       //необходимо обновить ассоциированные буферы
    bool             need_update_background;        //необходимо обновить параметры фона
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
    Границы логически отображаемой области экрана
*/

void RenderTarget::SetScreenArea (const Rect& rect)
{
  try
  {
    impl->SetScreenArea (rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetScreenArea");
    throw;
  }
}

void RenderTarget::SetScreenArea (int left, int top, size_t width, size_t height)
{
  SetScreenArea (Rect (left, top, width, height));
}

const Rect& RenderTarget::ScreenArea () const
{
  return impl->ScreenArea ();
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
    Экран (политика владения - weak-ref)
*/

void RenderTarget::SetScreen (render::Screen* screen)
{
  try
  {
    impl->SetScreen (screen);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTarget::SetScreen");
    throw;
  }
}

Screen* RenderTarget::Screen ()
{
  return impl->Screen ();
}

const Screen* RenderTarget::Screen () const
{
  return impl->Screen ();
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

void RenderTarget::CaptureImage (media::Image& image)
{
  try
  {
    impl->CaptureImage (image);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::CaptureImage(media::Image&)");
    throw;
  }
}

void RenderTarget::CaptureImage (const char* image_name)
{
  try
  {
    media::Image image;
    
    CaptureImage (image);
    
    image.Save (image_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::CaptureImage(const char*)");
    throw;
  }
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
