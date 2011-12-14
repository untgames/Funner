#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;

typedef stl::hash_map<stl::string, RenderTarget> RenderTargetMap;

/*
    Описание реализации области отрисовки
*/

struct RenderableView::Impl: public scene_graph::IViewportListener, public scene_graph::IScreenListener
{
  RenderManager&                    manager;                      //менеджер рендеринга
  scene_graph::Screen&              screen;                       //экран
  scene_graph::Viewport&            viewport;                     //область вывода
  ISceneRenderPtr                   renderer;                     //рендер сцены
  RenderTargetMap                   render_targets;               //области вывода
  common::PropertyMap::EventHandler properties_update_handler; //обработчик обновления свойств
  xtl::auto_connection              properties_update_connection; //соединение обновления свойств
  bool                              is_active;                    //активна ли области отрисовки
  bool                              need_reconfiguration;         //конфигурация изменена
  bool                              need_update_renderer;         //требуется обновить рендер
  bool                              need_update_render_targets;   //требуется обновить буферы отрисовки
  bool                              need_update_background;       //требуется обновить параметры очистки
  bool                              need_update_camera;           //требуется обновить камеру
  bool                              need_update_properties;       //требуется обновление свойств
  
///Конструктор
  Impl (RenderManager& in_manager, scene_graph::Screen& in_screen, scene_graph::Viewport& in_viewport)
    : manager (in_manager)
    , screen (in_screen)
    , viewport (in_viewport)
    , properties_update_handler (xtl::bind (&Impl::OnPropertiesUpdate, this))
    , is_active (viewport.IsActive ())
    , need_reconfiguration (true)
    , need_update_renderer (true)
    , need_update_render_targets (true)    
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
  {        
    properties_update_connection = viewport.Properties ().RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler);
    
    screen.AttachListener (this);
    
    try
    {
      viewport.AttachListener (this);
    }
    catch (...)
    {
      screen.DetachListener (this);
      throw;
    }
  }  
  
///Деструктор
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);
    }
    catch (...)
    {
    }
    
    try
    {
      screen.DetachListener (this);
    }
    catch (...)
    {
    }    
  }

///Область экрана обновлена
  void OnScreenChangeArea (const Rect&)
  {
    need_reconfiguration       = true;    
    need_update_render_targets = true;
  }

///Область вывода обновлена
  void OnViewportChangeArea (const Rect&)
  {
    need_reconfiguration       = true;
    need_update_render_targets = true;
  }

///Изменена камера
  void OnViewportChangeCamera (scene_graph::Camera* new_camera)
  {
    need_reconfiguration = true;
    need_update_camera   = true;
  }  

///Изменена активность области вывода
  void OnViewportChangeActive (bool new_state)
  {
    is_active = new_state;
  }
  
///Изменены параметры очистки области вывода
  void OnViewportChangeBackground (bool, const math::vec4f&)
  {
    need_reconfiguration   = true;
    need_update_background = true;
  }

///Изменен путь рендеринга
  void OnViewportChangeRenderer (const char*)
  {
    need_reconfiguration = true;
    need_update_renderer = true;
  }

///Изменены свойства рендеринга
  void OnViewportChangeProperties (const common::PropertyMap& properties)
  {
    need_update_properties = true;
    need_reconfiguration   = true;

    properties_update_connection = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler);
  }

  void OnPropertiesUpdate ()
  {
    need_update_properties = true;
  }
  
///Переконфигурация
  void Reconfigure ()
  {
    try
    {
      if (!need_reconfiguration)
        return;
        
        //переконфигурация рендера
        
      if (!renderer || need_update_renderer)
      {        
        renderer = ISceneRenderPtr ();

        renderer = SceneRenderManagerSingleton::Instance ()->CreateRender (manager, viewport.Technique ());
        
        if (!renderer)
          throw xtl::format_operation_exception ("", "Can't create sce render for technique '%s'", viewport.Technique ());
        
        need_update_renderer       = false;
        need_update_render_targets = true;
        need_update_camera         = true;
        need_update_properties     = true;
        need_update_background     = true;
      }
            
        //переконфигурация параметров очистки
        
      if (need_update_background)
      {
        render::Frame& frame = renderer->Frame ();

        if (viewport.BackgroundState ())
        {
          frame.SetClearColor (viewport.BackgroundColor ());          
          frame.SetClearFlags (render::ClearFlag_All | render::ClearFlag_ViewportOnly);
        }
        else
        {
          frame.SetClearFlags (0u);
        }

        need_update_background = false;
      }

        //переконфигурация камеры

      if (need_update_camera)
      {
        renderer->UpdateCamera (viewport.Camera ());

        need_update_camera = false;
      }

        //переконфигурация целевых буферов отрисовки
        
      if (need_update_render_targets && renderer)
      {        
        const scene_graph::Rect &screen_area   = screen.Area (),
                                &viewport_area = viewport.Area ();
                                
        Frame& frame = renderer->Frame ();

        for (RenderTargetMap::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
        {
          size_t target_width = iter->second.Width (), target_height = iter->second.Height ();

          double x_scale = screen_area.width ? double (target_width) / screen_area.width : 0.0,
                 y_scale = screen_area.height ? double (target_height) / screen_area.height : 0.0;

          render::Rect target_rect (int ((viewport_area.left () - screen_area.left ()) * x_scale),
                               int ((viewport_area.top () - screen_area.top ()) * y_scale),
                               size_t (ceil (viewport_area.width * x_scale)),
                               size_t (ceil (viewport_area.height * y_scale)));                               

          frame.SetRenderTarget (iter->first.c_str (), iter->second, render::Viewport (target_rect));          
        }
        
        need_update_render_targets = false;
      }
      
        //переконфигурация области вывода
      
      need_reconfiguration = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::RenderableView::Impl::Reconfigure");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

RenderableView::RenderableView (RenderManager& manager, scene_graph::Screen& screen, scene_graph::Viewport& viewport)
{
  try
  {
    impl = new Impl (manager, screen, viewport);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::RenderableView");
    throw;
  }
}

RenderableView::~RenderableView ()
{
}

/*
    Область вывода / экран
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

const scene_graph::Screen& RenderableView::Screen ()
{
  return impl->screen;
}

/*
    Регистрация целевых буферов отрисовки
*/

void RenderableView::SetRenderTarget (const char* name, const RenderTarget& target)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    RenderTargetMap::iterator iter = impl->render_targets.find (name);
    
    if (iter != impl->render_targets.end ())
    {
      iter->second = target;
    }
    else
    {
      impl->render_targets.insert_pair (name, target);
    }
    
    impl->need_reconfiguration       = true;
    impl->need_update_render_targets = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::SetRenderTarget");
    throw;
  }
}

void RenderableView::RemoveRenderTarget (const char* name)
{
  try
  {
    if (!name)
      return;

    RenderTargetMap::iterator iter = impl->render_targets.find (name);

    if (iter == impl->render_targets.end ())
      return;

    impl->render_targets.erase (iter);

    if (impl->renderer)
      impl->renderer->Frame ().RemoveRenderTarget (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::RemoveRenderTarget");
    throw;
  }
}

/*
    Обновление кадра
*/

void RenderableView::UpdateFrame (Frame* parent_frame)
{
  try
  {
      //отрисовка только в случае активности области вывода
    
    if (!impl->is_active)
      return;

      //переконфигурация в случае изменения существенных параметров

    if (impl->need_reconfiguration)
      impl->Reconfigure ();
      
      //в случае отсутствия рендера отрисовка не производится
      
    if (!impl->renderer)
      return;
      
      //переконфигурация дополнительных параметров
      
    if (impl->need_update_properties)
    {
      impl->renderer->UpdateProperties (impl->viewport.Properties ());

      impl->need_update_properties = false;
    }
    
      //построение кадра
      
    impl->renderer->UpdateFrame ();
    
      //отрисовка
      
    Frame& frame = impl->renderer->Frame ();
    
    if (frame.EntitiesCount () || frame.FramesCount ())
    {
      if (parent_frame) parent_frame->AddFrame (frame);
      else              frame.Draw ();    
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::RenderableView::Update");
    throw;
  }
}
