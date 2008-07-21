#include "scene_render_shared.h"

using namespace render;

/*
    Конструктор / деструктор
*/

RenderView::RenderView (const render::Viewport& vp, IRenderTargetAPI& in_render_target_api)
  : viewport (vp),
    render_target_api (in_render_target_api),
    current_scene (0),
    current_camera (0),
    need_update_view (true),
    need_update_area (true),
    need_update_camera (true),
    need_update_path (true)
{
  viewport.AttachListener (this);
  
  render_target_api.UpdateOrderNotify ();
}

RenderView::~RenderView ()
{
  viewport.DetachListener (this);
}    

/*
    Отрисовка
*/

void RenderView::Draw ()
{
  if (need_update_view)
    UpdateRenderView ();

  if (!render_view || !viewport.IsActive ())
    return;

  render_view->Draw ();
}

/*
    Обновление параметров области рендеринга
*/

void RenderView::UpdateRenderView ()
{
  try
  {
      //определение сцены

    scene_graph::Camera* camera = viewport.Camera ();
    scene_graph::Scene*  scene  = camera ? camera->Scene () : 0;
    
    if (camera && current_camera != camera)
    {
        //подписка на изменение сцены в камере

      on_camera_scene_change = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterSceneChange,
        xtl::bind (&RenderView::OnChangeScene, this));

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
        //получение менеджера путей рендеринга
        
      RenderPathManager* render_path_manager = render_target_api.GetRenderPathManager ();

        //при отсутствии камеры, сцены или менеджера путей рендеринга невозможно создать область рендеринга

      if (!camera || !scene || !render_path_manager)
        return;            

        //создание области рендеринга

      ICustomSceneRender& render = render_path_manager->GetRenderPath (viewport.RenderPath ());

      RenderViewPtr new_render_view (render.CreateRenderView (scene), false);

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
      const Rect& render_target_area = render_target_api.GetRenderTargetArea ();
      
      float kx = 1.0f / render_target_area.width,
            ky = 1.0f / render_target_area.height;

      const Rect& viewport_rect = viewport.Area ();

      render_view->SetViewport (render_target_area.left * kx, render_target_area.top * ky,
        viewport_rect.width * kx, viewport_rect.height * ky);

      need_update_area = false;
    }

    need_update_view = false;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderView::UpdateRenderView");

    throw;
  }
}

/*
    Сброс ресурсов
*/

void RenderView::FlushResources ()
{
  need_update_view = true;
  need_update_area = true;
  need_update_path = true;
  
  render_view = 0;
}

/*
    Обработчики событий
*/

//оповещение об изменении текущей камеры
void RenderView::OnChangeCamera (scene_graph::Camera*)
{
  need_update_view   = true;
  need_update_camera = true;

  on_camera_scene_change.disconnect ();
}

//оповещение о необходимости изменения границ области вывода (приходит от RenderTarget)
void RenderView::UpdateAreaNotify ()
{
  need_update_view = true;
  need_update_area = true;
}

//оповещение о необходимости изменения границ области вывода (приходит от Viewport)
void RenderView::OnChangeArea (const Rect&)
{
  UpdateAreaNotify ();
}

//оповещение о необходимости изменения пути рендеринга
void RenderView::OnChangeRenderPath (const char*)
{
  need_update_view = true;
  need_update_path = true;
}

//оповещение об изменении порядка следования областей вывода
void RenderView::OnChangeZOrder (int)
{
  render_target_api.UpdateOrderNotify ();
}

//оповещение о смене сцены
void RenderView::OnChangeScene ()
{
  need_update_view = true;
}

//оповещение об изменении свойства рендеринга
void RenderView::OnChangeProperty (const char* name, const char* value)
{
  if (!render_view)
    return;

  render_view->SetProperty (name, value);
}

/*
    Подсчёт ссылок (не требуется, регистрация слушателя будет отменена при удалении RenderView или текущей RenderTarget)
*/

void RenderView::AddRef ()
{
}

void RenderView::Release ()
{
}
