#include "shared.h"

using namespace render;

/*
    Описание реализации отобржаемой области вывода
*/

class RenderableViewport::Impl: private IViewportListener, public xtl::reference_counter
{
  public:
///Конструктор
    Impl (const render::Viewport& in_viewport, RenderTargetImpl& in_render_target, RenderManager& in_render_manager)
      : viewport (in_viewport),
        render_target (in_render_target),
        render_manager (in_render_manager),
        renderer (in_render_manager.Renderer ()),
        current_scene (0),
        current_camera (0),
        need_update_view (true),
        need_update_area (true),
        need_update_camera (true),
        need_update_path (true),
        need_update_clear_frame (true)
    {
        //подписка на события области вывода

      viewport.AttachListener (this);

        //оповещение о необходимости пересортировки областей вывода

      render_target.UpdateViewportsOrder ();
    }

///Деструктор
    ~Impl ()
    {
        //отмена регистрации слушателя

      viewport.DetachListener (this);
    }
    
///Получение области вывода
    render::Viewport& Viewport () { return viewport; }
    
///Обновление области вывода
    void Update ()
    {
        //обновление области рендеринга

      if (need_update_view)
        UpdateRenderView ();
        
        //обновление очищающего кадра
        
      if (need_update_clear_frame)
        UpdateClearFrame ();

      if (!render_view || !viewport.IsActive ())
        return;

        //очистка кадра

      if (viewport.BackgroundState ())
      {
        render_manager.Renderer ().AddFrame (clear_frame.get ());
      }

        //визуализация

      render_view->Draw ();
    }

///Оповещение о необходимости изменения границ области вывода
    void UpdateArea ()
    {
      need_update_view        = true;
      need_update_area        = true;
      need_update_clear_frame = true;
    }

  private:
///Обновление параметров области вывода
    void UpdateRenderView ()
    {
      try
      {
          //определение обязательных объектов

        scene_graph::Camera* camera = viewport.Camera ();
        scene_graph::Scene*  scene  = camera ? camera->Scene () : 0;
        Screen*              screen = render_target.Screen ();
        
        if (!screen)
          throw xtl::format_operation_exception ("", "Null screen");

        if (camera && current_camera != camera)
        {
            //подписка на изменение сцены в камере

          on_camera_scene_change = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterSceneChange,
            xtl::bind (&Impl::OnChangeScene, this));

          current_camera = camera;
        }      

        if (need_update_path || scene != current_scene) //условие пересоздания области вывода
        {
          render_view      = 0;
          current_scene    = 0;
          need_update_path = need_update_camera = need_update_view = need_update_area = false;
        }

            //пересоздание области вывода

        if (!render_view)
        {
            //при отсутствии камеры или сцены невозможно создать область рендеринга

          if (!camera || !scene)
            return;            

            //создание области рендеринга

          ICustomSceneRender& render_path = render_manager.GetRenderPath (viewport.RenderPath ());

          RenderViewPtr new_render_view (render_path.CreateRenderView (scene), false);

            //установка целевых буферов рендеринга

          new_render_view->SetRenderTargets (render_target.ColorAttachment (), render_target.DepthStencilAttachment ());

            //установка свойств области вывода в область рендеринга

          for (Viewport::PropertyIterator prop_iter=viewport.CreatePropertyIterator (); prop_iter; ++prop_iter)
            new_render_view->SetProperty (prop_iter->Name (), prop_iter->Value ());        

            //явное разрешение обновления всех параметров вывода

          need_update_area = need_update_camera = true;
          need_update_path = false;
          render_view      = new_render_view;            
        }

          //обновление камеры

        if (need_update_camera)
        {
          render_view->SetCamera (camera);

          current_scene = scene;

          need_update_camera = false;
        }

          //обновление параметров области вывода
          
        if (need_update_area)
        {
          const Rect &renderable_area = render_target.RenderableArea (),
                     &screen_area     = screen->Area (),
                     &viewport_area   = viewport.Area ();

          float x_scale  = float (renderable_area.width) / screen_area.width,
                y_scale  = float (renderable_area.height) / screen_area.height;
                
          Rect result (int (renderable_area.left + (viewport_area.left - screen_area.left) * x_scale),
                       int (renderable_area.top + (viewport_area.top - screen_area.top) * y_scale),
                       size_t (ceil (viewport_area.width * x_scale)),
                       size_t (ceil (viewport_area.height * y_scale)));

          render_view->SetViewport (result);

          need_update_area = false;
        }

        need_update_view = false;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::RenderableViewport::UpdateRenderView");

        throw;
      }
    }
    
///Обновление очищающего кадра
    void UpdateClearFrame ()
    {
        //сброс очищающего кадра

      if (!viewport.BackgroundState () || !render_view)
      {
        if (clear_frame)
          clear_frame = 0;

        return;
      }

        //создание очищающего кадра (при необходимости)

      if (!clear_frame)
      {
        clear_frame = ClearFramePtr (renderer.CreateClearFrame (), false);

        clear_frame->SetRenderTargets (render_target.ColorAttachment (), render_target.DepthStencilAttachment ());
        clear_frame->SetFlags         (render::mid_level::ClearFlag_All | render::mid_level::ClearFlag_ViewportOnly);
      }

          //установка начальной области вывода

      Rect src_viewport_rect;

      render_view->GetViewport (src_viewport_rect);

      render::mid_level::Viewport dst_viewport_rect;

      dst_viewport_rect.x      = src_viewport_rect.left;
      dst_viewport_rect.y      = src_viewport_rect.top;
      dst_viewport_rect.width  = src_viewport_rect.width;
      dst_viewport_rect.height = src_viewport_rect.height;

      clear_frame->SetViewport (dst_viewport_rect);

        //обновление цвет очистки

      clear_frame->SetColor (viewport.BackgroundColor ());

        //снятие флага необходимости обновления очищающего кадра

      need_update_clear_frame = false;
    }

///Оповещение об изменении текущей камеры
    void OnChangeCamera (scene_graph::Camera*)
    {
      need_update_view   = true;
      need_update_camera = true;

      on_camera_scene_change.disconnect ();
    }

///Оповещение о необходимости изменения границ области вывода
    void OnChangeArea (const Rect&)
    {
      UpdateArea ();
    }

///Оповещение о необходимости изменения пути рендеринга
    void OnChangeRenderPath (const char*)
    {
      need_update_view = true;
      need_update_path = true;
    }

///Оповещение об изменении порядка следования областей вывода
    void OnChangeZOrder (int)
    {
      render_target.UpdateViewportsOrder ();
    }

///Оповещение о смене сцены
    void OnChangeScene ()
    {
      need_update_view = true;
    }

///Оповещение об изменении свойства рендеринга
    void OnChangeProperty (const char* name, const char* value)
    {
      if (!render_view)
        return;

      render_view->SetProperty (name, value);
    }

///Оповещение об изменении фона
    void OnChangeBackground (bool, const math::vec4f&)
    {
      need_update_clear_frame = true;
    }

  private:
    typedef xtl::com_ptr<IRenderView>                    RenderViewPtr;
    typedef xtl::com_ptr<render::mid_level::IClearFrame> ClearFramePtr;
    typedef xtl::com_ptr<render::mid_level::IRenderer>   RendererPtr;

  private:
    render::Viewport      viewport;                //область вывода
    RenderTargetImpl&     render_target;           //цель рендеринга
    RenderManager&        render_manager;          //менеджер рендеринга
    mid_level::IRenderer& renderer;                //система рендеринга
    RenderViewPtr         render_view;             //область рендеринга сцены
    scene_graph::Scene*   current_scene;           //текущая сцена
    scene_graph::Camera*  current_camera;          //текущая камера
    ClearFramePtr         clear_frame;             //кадр очистки
    bool                  need_update_view;        //необходимо обновить параметры области вывода
    bool                  need_update_area;        //необходимо обновить координаты области вывода
    bool                  need_update_camera;      //необходимо обновить камеру
    bool                  need_update_path;        //необходимо обновить путь рендеринга
    bool                  need_update_clear_frame; //необходимо обновить очищающий кадр
    xtl::auto_connection  on_camera_scene_change;  //соединение с сигналом оповещения об изменении сцены в камере
};

/*
    Конструкторы / деструктор / присваивание
*/

RenderableViewport::RenderableViewport (const render::Viewport& viewport, RenderTargetImpl& render_target, RenderManager& render_manager)
{
  try
  {
    impl = new Impl (viewport, render_target, render_manager); 
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderableViewport::RenderableViewport");
    throw;
  }
}

RenderableViewport::RenderableViewport (const RenderableViewport& view)
  : impl (view.impl)
{
  addref (impl);
}

RenderableViewport::~RenderableViewport ()
{
  release (impl);
}

RenderableViewport& RenderableViewport::operator = (const RenderableViewport& view)
{
  RenderableViewport new_view (view);

  stl::swap (impl, new_view.impl);

  return *this;
}

/*
    Получение области вывода
*/

render::Viewport& RenderableViewport::Viewport ()
{
  return impl->Viewport ();
}

/*
    Обновление област вывода
*/

void RenderableViewport::Update ()
{
  impl->Update ();
}

/*
    Оповещение о необходимости изменения границ области вывода
*/

void RenderableViewport::UpdateArea ()
{
  impl->UpdateArea ();
}
