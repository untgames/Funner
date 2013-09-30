#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации области вывода
*/

struct RenderableView::Impl: public scene_graph::IViewportListener
{
  ConnectionPtr           connection;                   //соединение
  scene_graph::Viewport&  viewport;                     //ссылка на область вывода
  PropertyMapSynchronizer properties_sync;              //синхронизатор свойств
  object_id_t             id;                           //идентификатор области вывода
  bool                    is_active;                    //активна ли области отрисовки
  bool                    need_reconfiguration;         //конфигурация изменена
  bool                    need_update_renderer;         //требуется обновить рендер
  bool                    need_update_background;       //требуется обновить параметры очистки
  bool                    need_update_camera;           //требуется обновить камеру
  bool                    need_update_properties;       //требуется обновление свойств
  bool                    need_update_activity;         //требуется обновление активности области вывода
  bool                    need_update_name;             //требуется обновление имени
  bool                    need_update_area;             //требуется обновление области вывода

/// Конструктор
  Impl (const ConnectionPtr& in_connection, scene_graph::Viewport& in_viewport)
    : connection (in_connection)
    , viewport (in_viewport)
    , id ()
    , is_active (viewport.IsActive ())
    , need_reconfiguration (true)
    , need_update_renderer (true)
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
    , need_update_activity (true)
    , need_update_name (true)
    , need_update_area (true)
  {
    if (!connection)
      throw xtl::make_null_argument_exception ("", "connection");

    id = connection->Client ().AllocateId (ObjectType_Viewport);

    connection->Context ().CreateViewport (id);

    try
    {
      properties_sync = connection->Client ().CreateSynchronizer (viewport.Properties ());

      connection->Context ().SetViewportProperties (id, viewport.Properties ().Id ());

      viewport.AttachListener (this);
    }
    catch (...)
    {
      connection->Context ().DestroyViewport (id);
      throw;
    }
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      viewport.DetachListener (this);

      connection->Context ().DestroyViewport (id);

      connection->Client ().DeallocateId (ObjectType_Viewport, id);
    }
    catch (...)
    {
    }
  }

///Область вывода обновлена
  void OnViewportChangeArea (const scene_graph::Rect&)
  {
    need_reconfiguration = true;
    need_update_area     = true;
  }

///Имя обновлено
  void OnViewportChangeName (const char*)
  {
    need_reconfiguration = true;
    need_update_name     = true;
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
    need_update_activity = true;
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

      Context& context = connection->Context ();

        //переконфигурация имени

      if (need_update_name)
      {
        context.SetViewportName (id, viewport.Name ());

        need_update_name = false;
      }
        
        //переконфигурация области вывода

      if (need_update_area)
      {
        const scene_graph::Rect& r = viewport.Area ();

        context.SetViewportArea (id, r.x, r.y, r.width, r.height);
      }

        //переконфигурация рендера
        
      if (need_update_renderer)
      {        
        context.SetViewportTechnique (id, viewport.Technique ());

        need_update_renderer = false;
      }

        //переконфигурация активности

      if (need_update_activity)
      {
        context.SetViewportActive (id, viewport.IsActive ());

        need_update_activity = false;
      }
            
        //переконфигурация параметров очистки
        
      if (need_update_background)
      {
        context.SetViewportBackground (id, viewport.BackgroundState (), viewport.BackgroundColor ());

        need_update_background = false;
      }

        //переконфигурация камеры

      if (need_update_camera)
      {
        //?????????????

        need_update_camera = false;
      }
      
        //переконфигурация свойств

      if (need_update_properties)
      {        
        properties_sync = connection->Client ().CreateSynchronizer (viewport.Properties ());

        connection->Client ().Synchronize ();

        context.SetViewportProperties (id, viewport.Properties ().Id ());

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
    Идентификатор
*/

object_id_t RenderableView::Id ()
{
  return impl->id;
}

/*
    Синхронизация с сервером
*/

void RenderableView::Synchronize ()
{
  try
  {
    impl->connection->Client ().Synchronize ();

    impl->Synchronize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderableView::Synchronize");
    throw;
  }
}
