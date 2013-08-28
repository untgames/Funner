#include "shared.h"

using namespace render::scene::client;

/*
    Константы
*/

const size_t RESERVE_VIEWS_COUNT = 8; //резервируемое количество областей вывода

/*
    Описание реализации цели рендеринга
*/

typedef stl::vector<RenderableViewPtr> ViewList;

struct RenderTargetImpl::Impl: public scene_graph::IScreenListener
{
  stl::string          name;       //имя цели
  ConnectionPtr        connection; //соединение
  scene_graph::Screen* screen;     //экран
  ViewList             views;      //области вывода

/// Конструктор
  Impl (const ConnectionPtr& in_connection, const char* render_target_name)
    : connection (in_connection)
    , screen ()    
  {
    static const char* METHOD_NAME = "render::scene::client::RenderTargetImpl::RenderTargetImpl";

    if (!connection)
      throw xtl::make_null_argument_exception (METHOD_NAME, "connection");

    if (!render_target_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "render_target_name");

    name = render_target_name;

    views.reserve (RESERVE_VIEWS_COUNT);
  }
  
///Параметры заднего фона экрана изменены
  void OnScreenChangeBackground (bool state, const math::vec4f& new_color)
  {
    //TODO: change screen background
  }

///Присоединена область вывода
  void OnScreenAttachViewport (scene_graph::Viewport& viewport)
  {
    try
    {
      if (!screen)
        return;

      RenderableViewPtr view (new RenderableView (connection, viewport), false);

      views.push_back (view);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderTargetImpl::Impl::OnScreenAttachViewport");
      throw;
    }
  }

///Отсоединена область вывода
  void OnScreenDetachViewport (scene_graph::Viewport& viewport)
  {
    try
    {
      size_t id = viewport.Id ();
      
      for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        if ((*iter)->Viewport ().Id () == id)
        {
          views.erase (iter);

          return;
        }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderTargetImpl::Impl::OnScreenDetachViewport");
      throw;
    }    
  }
  
///Инициализация
  void Initialize ()
  {
    try
    {
      if (!screen)
        return;
        
      for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
        OnScreenAttachViewport (screen->Viewport (i));
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderTargetImpl::Impl::Initialize");
      throw;
    }
  }
  
///Завершение
  void Destroy ()
  {
    try
    {
      while (!views.empty ())
        views.pop_back ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderTargetImpl::Impl::Destroy");
      throw;
    }
  }

///Экран удален
  void OnScreenDestroy ()
  {
    try
    {
      Destroy ();
      
      screen = 0;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::RenderTargetImpl::Impl::OnScreenDestroy");
      throw;
    }
  }
};

/*
  Конструктор / деструктор
*/

RenderTargetImpl::RenderTargetImpl (const ConnectionPtr& connection, const char* render_target_name)
  : impl (new Impl (connection, render_target_name))
{
}

RenderTargetImpl::~RenderTargetImpl ()
{
  try
  {
    SetScreen (0);
  }
  catch (...)
  {
  }
}

/*
    Экран (политика владения - weak-ref)
*/

void RenderTargetImpl::SetScreen (scene_graph::Screen* screen)
{
  try
  {
    if (screen == impl->screen)
      return;

    if (impl->screen)
    {
      impl->Destroy ();

      impl->screen->DetachListener (&*impl);

      impl->screen = 0;
    }

    impl->screen = screen;

    try
    {      
      impl->Initialize ();

      impl->screen->AttachListener (&*impl);
    }
    catch (...)
    {
      impl->Destroy ();

      impl->screen = 0;
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTargetImpl::SetScreen");
    throw;
  }
}

scene_graph::Screen* RenderTargetImpl::Screen ()
{
  return impl->screen;
}

/*
    Обновление
*/

void RenderTargetImpl::Update ()
{
  try
  {
      //обновление
    
    for (ViewList::iterator iter=impl->views.begin (), end=impl->views.end (); iter!=end; ++iter)
      (*iter)->Synchronize ();

      //отсылка команды отрисовки цели

    //???

      //отсылка команд на выполнение

    impl->connection->Context ().Flush ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::RenderTargetImpl::Update");
    throw;
  }
}

/*
    Захват изображения (screen-shot)
*/

void RenderTargetImpl::CaptureImage (media::Image&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
