#include "shared.h"

using namespace render;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    Константы
*/

const size_t VIEW_ARRAY_RESERVE = 8; //резервируемое количество областей видимости

/*
    Транзакция отрисовки
*/

struct DrawScope
{
  DrawTransactionManager& draw_transaction_manager;   //менеджер транзакций отрисовки
  size_t                  transaction_id;             //идентификатор транзакции
  size_t&                 last_update_transaction_id;

  DrawScope (DrawTransactionManager& manager, size_t& in_last_update_transaction_id)
    : draw_transaction_manager (manager),
      transaction_id (manager.BeginDraw (in_last_update_transaction_id)),
      last_update_transaction_id (in_last_update_transaction_id) {}

  ~DrawScope ()
  {
    if (transaction_id)
    {
      draw_transaction_manager.EndDraw ();

      last_update_transaction_id = transaction_id;
    }
  }    
};

}

/*
    Описание реализации цели рендеринга
*/

struct RenderTargetImpl::Impl: private IScreenListener
{
  public:
///Конструктор
    Impl (RenderTargetImpl&             in_owner,
          mid_level::IRenderTarget* in_color_attachment,
          mid_level::IRenderTarget* in_depth_stencil_attachment)
      : owner (in_owner),
        render_manager (0),
        color_attachment (in_color_attachment),
        depth_stencil_attachment (in_depth_stencil_attachment),
        screen (0),
        last_update_transaction_id (0),
        need_update_background (true),
        need_update_areas (true),
        need_reorder (true)
    {
        //резервирование памяти для хранения областей вывода

      views.reserve (VIEW_ARRAY_RESERVE);

        //установка начальной физической области вывода
        
      mid_level::IRenderTarget* render_target = color_attachment ? color_attachment.get () : depth_stencil_attachment.get ();

      if (render_target)
        SetRenderableArea (Rect (0, 0, render_target->GetWidth (), render_target->GetHeight ()));
    }
    
///Деструктор
    ~Impl ()
    {
        //отмена регистрации слушателя

      SetScreen (0);

        //отмена регистрации в менеджере

      SetRenderManager (0);
    }
    
///Целевые буферы
    mid_level::IRenderTarget* ColorAttachment () { return color_attachment.get (); }
    mid_level::IRenderTarget* DepthStencilAttachment () { return depth_stencil_attachment.get (); }

///Установка текущего менеджера рендеринга
    void SetRenderManager (render::RenderManager* new_render_manager)
    {
      if (new_render_manager == render_manager)
        return;

        //очистка ресурсов

      views.clear ();            
      
        //установка флагов

      need_update_areas          = true;
      need_reorder               = true;
      need_update_background     = true;
      last_update_transaction_id = 0;
      
        //отмена регистрации в старом менеджере

      if (render_manager)
      {
        render_manager->UnregisterRenderTarget (&owner);

        render_manager = 0;
      }
      
      if (new_render_manager)
      {
          //регистрация в новом менеджере

        new_render_manager->RegisterRenderTarget (&owner);

          //изменение менеджера рендеринга

        render_manager = new_render_manager;

          //регистрация областей вывода          

        AddViewports ();        
      }
    }
    
///Текущий менеджер рендеринга
    render::RenderManager* RenderManager () { return render_manager; }

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

        //установка флагов

      need_update_areas          = true;
      need_reorder               = true;
      need_update_background     = true;
      last_update_transaction_id = 0;

        //регистрация областей вывода

      if (screen)
      {
        AddViewports ();

          //регистрация слушателя

        screen->AttachListener (this);
      }
    }
   
///Изменение физическое окна вывода
    void SetRenderableArea (const Rect& rect)
    {
      if (renderable_area.left == rect.left && renderable_area.top == rect.top && renderable_area.width == rect.width && renderable_area.height == rect.height)
        return;      

      renderable_area   = rect;
      need_update_areas = true;
    }

///Текущее физическое окно вывода
    const Rect& RenderableArea () { return renderable_area; }
    
///Оповещение о необходимости пересортировки областей вывода
    void UpdateViewportsOrder ()
    {
      need_reorder = true;
    }

///Обновление цели рендеринга
    void Update ()
    {
      static const char* METHOD_NAME = "render::RenderTargetImpl::Impl::Update";      

      if (!screen || !render_manager)
        return;        
        
      size_t transaction_id = 0;

      try
      { 
          //открытие транзакции отрисовки

        DrawScope draw_scope (render_manager->DrawTransactionManager (), last_update_transaction_id);       

        if (!draw_scope.transaction_id)
          return; //транзакция отклонена
          
          //упорядочивание областей вывода          

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
          Viewport& viewport = iter->Viewport ();        
          
          try
          {
            iter->Update ();
          }
          catch (std::exception& exception)
          {
            render_manager->LogPrintf ("%s\n    at draw viewport='%s', render_path='%s'", exception.what (), viewport.Name (), viewport.RenderPath ());
          }
          catch (...)
          {
            render_manager->LogPrintf ("Unknown exception\n    at draw viewport='%s', render_path='%s'", viewport.Name (), viewport.RenderPath ());
          }
        }
      }
      catch (std::exception& exception)
      {
        render_manager->LogPrintf ("%s\n    at RenderTargetImpl::Draw", exception.what ());
      }
      catch (...)
      {
        render_manager->LogPrintf ("Unknown exception\n    at RenderTargetImpl::Draw");
      }
    }    

///Захват изображения
    void CaptureImage (media::Image& image)
    {      
      if (!color_attachment)
        throw xtl::format_operation_exception ("render::RenderTargetImpl::Impl::CaptureImage", "Null color attachment");

      color_attachment->CaptureImage (image);
    }
    
  private:
///Добавление областей вывода
    void AddViewports ()
    {
      if (!screen)
        return;

      for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
        OnAttachViewport (screen->Viewport (i));
    }

///Сортировка областей вывода
    void SortViews ()
    {
      struct ViewComparator
      {
        bool operator () (const RenderableViewport& view1, const RenderableViewport& view2) const
        {
          return const_cast<RenderableViewport&> (view1).Viewport ().ZOrder () < const_cast<RenderableViewport&> (view2).Viewport ().ZOrder ();
        }
      };

      stl::sort (views.begin (), views.end (), ViewComparator ());

      need_reorder = false;    
    }

///Обновление границ областей вывода
    void UpdateAreas ()
    {
      for (ViewArray::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        iter->UpdateArea ();

      need_update_areas = false;
    }

///Очистка цели рендеринга
    void ClearRenderTarget ()
    {
      try
      {
        if (!need_update_background && !clear_frame || !render_manager)
          return;

        mid_level::IRenderer& renderer = render_manager->Renderer ();

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

            clear_frame->SetRenderTargets (color_attachment.get (), depth_stencil_attachment.get ());
            clear_frame->SetFlags         (render::mid_level::ClearFlag_All);
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
        exception.touch ("render::RenderTargetImpl::Impl::ClearRenderTarget");
        throw;
      }
    }
    
///Оповещение об изменении рабочей области экрана
    void OnChangeArea (const Rect&)
    {
      need_update_areas = true;
    }
    
///Оповещение об изменении фона
    void OnChangeBackground (bool, const math::vec4f&)
    {
      need_update_background = true;
    }

///Оповещение о добавлении области вывода
    void OnAttachViewport (Viewport& viewport)
    {
      if (render_manager)
        views.push_back (RenderableViewport (viewport, owner, *render_manager));
    }

///Оповещение об удалении области вывода
    void OnDetachViewport (Viewport& viewport)
    {
      size_t id = viewport.Id ();

      for (ViewArray::iterator iter=views.begin (); iter!=views.end ();)
        if (iter->Viewport ().Id () == id) views.erase (iter);
        else                               ++iter;
    }

///Оповещение об удалении экрана
    void OnDestroy ()
    {
      SetScreen (0);
    }    

  private:
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;
    typedef stl::vector<RenderableViewport>              ViewArray;
    typedef xtl::com_ptr<mid_level::IRenderTarget>       AttachmentPtr;

  private:
    RenderTargetImpl&      owner;                     //владелец
    render::RenderManager* render_manager;            //менеджер рендеринга
    AttachmentPtr          color_attachment;          //ассоциированный буфер цвета
    AttachmentPtr          depth_stencil_attachment;  //ассоциированный буфер попиксельного отсечения
    render::Screen*        screen;                    //экран
    ViewArray              views;                     //массив областей рендеринга
    ClearFramePtr          clear_frame;               //очищающий кадр
    Rect                   renderable_area;           //границы отображаемой области
    size_t                 last_update_transaction_id;//идентификатор последней транзакции обновления
    bool                   need_update_background;    //необходимо обновить параметры фона
    bool                   need_update_areas;         //необходимо обновить границы областей вывода
    bool                   need_reorder;              //необходимо пересортировать области вывода
};

/*
    Конструктор / деструктор
*/

RenderTargetImpl::RenderTargetImpl
 (mid_level::IRenderTarget* render_target,
  mid_level::IRenderTarget* depth_stencil_target)
    : impl (new Impl (*this, render_target, depth_stencil_target))
{
}

RenderTargetImpl::~RenderTargetImpl ()
{
}

/*
    Создание
*/

RenderTargetImpl::Pointer RenderTargetImpl::Create
 (mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment)
{
  return Pointer (new RenderTargetImpl (color_attachment, depth_stencil_attachment), false);
}

RenderTargetImpl::Pointer RenderTargetImpl::Create
 (render::RenderManager&    render_manager,
  mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment)
{
  Pointer result = Create (color_attachment, depth_stencil_attachment);

  result->SetRenderManager (&render_manager);

  return result;
}

/*
    Целевые буферы рендеринга
*/

mid_level::IRenderTarget* RenderTargetImpl::ColorAttachment ()
{
  return impl->ColorAttachment ();
}

mid_level::IRenderTarget* RenderTargetImpl::DepthStencilAttachment ()
{
  return impl->DepthStencilAttachment ();
}

/*
    Менеджер рендеринга
*/

void RenderTargetImpl::SetRenderManager (render::RenderManager* render_manager)
{
  impl->SetRenderManager (render_manager);
}

RenderManager* RenderTargetImpl::RenderManager ()
{
  return impl->RenderManager ();
}

/*
    Экран (политика владения - weak-ref)
*/

void RenderTargetImpl::SetScreen (render::Screen* screen)
{
  try
  {
    impl->SetScreen (screen);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetImpl::SetScreen");
    throw;
  }
}

render::Screen* RenderTargetImpl::Screen ()
{
  return impl->Screen ();
}

/*
    Размеры области рендеринга
*/

const Rect& RenderTargetImpl::RenderableArea ()
{
  return impl->RenderableArea ();
}

void RenderTargetImpl::SetRenderableArea (const Rect& rect)
{
  impl->SetRenderableArea (rect);
}

/*
    Оповещение о необходимости пересортировки областей вывода
*/

void RenderTargetImpl::UpdateViewportsOrder ()
{
  impl->UpdateViewportsOrder ();
}

/*
    Обновление
*/

void RenderTargetImpl::Update ()
{
  try
  {
    impl->Update ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetImpl::Update");
    throw;
  }
}

/*
    Захват изображения (screen-shot)
*/

void RenderTargetImpl::CaptureImage (media::Image& image)
{
  try
  {
    impl->CaptureImage (image);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetImpl::CaptureImage(media::Image&)");
    throw;
  }
}

void RenderTargetImpl::CaptureImage (const char* image_name)
{
  try
  {
    media::Image image;

    CaptureImage (image);

    image.Save (image_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetImpl::CaptureImage(const char*)");
    throw;
  }
}
