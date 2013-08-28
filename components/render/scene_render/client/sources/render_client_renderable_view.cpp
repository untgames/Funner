#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации области вывода
*/

struct RenderableView::Impl: public scene_graph::IViewportListener
{
  ConnectionPtr                     connection;                   //соединение
  scene_graph::Viewport&            viewport;                     //ссылка на область вывода
  common::PropertyMap::EventHandler properties_update_handler;    //обработчик обновления свойств
  xtl::auto_connection              properties_update_connection; //соединение обновления свойств
  bool                              is_active;                    //активна ли области отрисовки
  bool                              need_reconfiguration;         //конфигурация изменена
  bool                              need_update_renderer;         //требуется обновить рендер
  bool                              need_update_render_targets;   //требуется обновить буферы отрисовки
  bool                              need_update_background;       //требуется обновить параметры очистки
  bool                              need_update_camera;           //требуется обновить камеру
  bool                              need_update_properties;       //требуется обновление свойств

/// Конструктор
  Impl (const ConnectionPtr& in_connection, scene_graph::Viewport& in_viewport)
    : connection (in_connection)
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
    if (!connection)
      throw xtl::make_null_argument_exception ("", "connection");

    properties_update_connection = viewport.Properties ().RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler);

    viewport.AttachListener (this);    
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);
    }
    catch (...)
    {
    }
  }

///Область вывода обновлена
  void OnViewportChangeArea (const scene_graph::Rect&)
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
  void OnViewportChangeActive (bool)
  {
    need_reconfiguration = true;
  }
  
///Изменены параметры очистки области вывода
  void OnViewportChangeBackground (bool, const math::vec4f&)
  {
    need_reconfiguration   = true;
    need_update_background = true;
  }

///Изменен путь рендеринга
  void OnViewportChangeTechnique (const char*)
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
    need_reconfiguration   = true;
  }
  
///Синхронизация
  void Synchronize ()
  {
    try
    {
      if (!need_reconfiguration)
        return;
        
        //переконфигурация рендера
        
      if (need_update_renderer)
      {        

        need_update_renderer       = false;
      }

        //переконфигурация активности

      if (is_active != viewport.IsActive ())
      {
        //????
      }
            
        //переконфигурация параметров очистки
        
      if (need_update_background)
      {

        need_update_background = false;
      }

        //переконфигурация камеры

      if (need_update_camera)
      {

        need_update_camera = false;
      }

        //переконфигурация целевых буферов отрисовки
        
      if (need_update_render_targets)
      {        
        
        need_update_render_targets = false;
      }     
      
        //переконфигурация свойств

      if (need_update_properties)
      {

        need_update_properties = false;
      }
      
      need_reconfiguration = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderableView::Impl::Synchronize");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

RenderableView::RenderableView (const ConnectionPtr& connection, scene_graph::Viewport& viewport)
{
  try
  {
    impl.reset (new Impl (connection, viewport));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::View::View");
    throw;
  }
}

RenderableView::~RenderableView ()
{
}

/*
    Область вывода
*/

const scene_graph::Viewport& RenderableView::Viewport ()
{
  return impl->viewport;
}

/*
    Синхронизация с сервером
*/

void RenderableView::Synchronize ()
{
  try
  {
    impl->Synchronize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderableView::Synchronize");
    throw;
  }
}
