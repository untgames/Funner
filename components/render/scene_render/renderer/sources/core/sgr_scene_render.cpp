#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

using namespace render;

//TODO: change screen background

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "render.scene_render"; //имя потока протоколирования

}

/*
    Описание реализации рендера сцены
*/

typedef stl::hash_map<stl::string, RenderTarget> RenderTargetMap;
typedef stl::vector<RenderableViewPtr>           ViewList;

struct SceneRender::Impl: public xtl::reference_counter, public scene_graph::IScreenListener, public scene_graph::IViewportListener
{
  render::RenderManager*     manager;                    //менеджер рендеринга связанный с ренедром сцены
  scene_graph::Screen*       screen;                     //экран
  RenderTargetMap            render_targets;             //целевые буферы рендеринга
  ViewList                   views;                      //список областей вывода
  bool                       need_sort_views;            //области вывода требуют сортировки
  common::Log                log;                        //поток протоколирования
  xtl::trackable::slot_type  on_destroy_manager;         //обработчик удаления менеджера рендеринга
  xtl::auto_connection       destroy_manager_connection; //соединение с обработчиком удаления менеджера рендеринга

///Конструктор
  Impl ()
    : manager ()
    , screen ()
    , need_sort_views (false)
    , log (LOG_NAME)
    , on_destroy_manager (xtl::bind (&Impl::OnDestroyManager, this))
  {    
  }

///Деструктор
  ~Impl ()
  {    
    try
    {
      Destroy ();

      RemoveAllRenderTargets ();          

      if (screen)
        screen->DetachListener (this);
    }
    catch (...)
    {      
    }
  }

///Установка целевого буфера
  void SetRenderTarget (const char* name, const render::RenderTarget& target)
  {
    try
    {
      if (!name)
        throw xtl::make_null_argument_exception ("", "name");
        
      RenderTargetMap::iterator iter = render_targets.find (name);
      
      if (iter != render_targets.end ())
      {
        iter->second = target;
        return;
      }            

      render_targets.insert_pair (name, target);

      for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->SetRenderTarget (name, target);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::SetRenderTarget");
      throw;
    }
  }
  
///Удаление целевого буфера
  void RemoveRenderTarget (const char* name)
  {
    try
    {
      if (!name)
        return;
        
      RenderTargetMap::iterator iter = render_targets.find (name);
      
      if (iter == render_targets.end ())
        return;        
        
      render_targets.erase (iter);
      
      for (ViewList::iterator iter=views.begin (), end=views.end (); iter!=end; ++iter)
        (*iter)->RemoveRenderTarget (name);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::RemoveRenderTarget");
      throw;
    }
  }  
  
///Удаление всех целевых буферов
  void RemoveAllRenderTargets ()
  {
    while (!render_targets.empty ())
      RemoveRenderTarget (render_targets.rbegin ()->first.c_str ());
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
      if (!manager)
        return;
        
      if (!screen)
        return;

      RenderableViewPtr view (new RenderableView (*manager, *screen, viewport), false);

      for (RenderTargetMap::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
        view->SetRenderTarget (iter->first.c_str (), iter->second);        
        
      views.push_back (view);
      
      try
      {
        viewport.AttachListener (this);
      }
      catch (...)
      {
        views.pop_back ();
        throw;
      }

      need_sort_views = true;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::OnScreenAttachViewport");
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
          (*iter)->Viewport ().DetachListener (this);

          views.erase (iter);

          return;
        }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::OnScreenAttachViewport");
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
        
      if (!manager)
        return;
        
      for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
        OnScreenAttachViewport (screen->Viewport (i));
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::Initialize");
      throw;
    }
  }
  
///Завершение
  void Destroy ()
  {
    try
    {
      while (!views.empty ())
      {
        views.back ()->Viewport ().DetachListener (this);

        views.pop_back ();
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::SceneRender::Impl::Destroy");
      throw;
    }
  }
  
///Оповещение об удалении менеджера рендеринга
  void OnDestroyManager ()
  {
    manager = 0;

    Destroy ();
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
      e.touch ("render::SceneRender::Impl::OnScreenDestroy");
      throw;
    }
  }
  
///Порядок вывода изменен
  void OnViewportChangeZOrder (int)
  {
    need_sort_views = true;
  }
  
///Компаратор областей вывода
  struct ViewComparator
  {
    bool operator () (const RenderableViewPtr& view1, const RenderableViewPtr& view2) const
    {
      return view1->Viewport ().ZOrder () > view2->Viewport ().ZOrder ();
    }
  };
  
///Сортировка областей вывода
  void SortViews ()
  {
    if (!need_sort_views)
      return;

    stl::sort (views.begin (), views.end (), ViewComparator ());

    need_sort_views = false;
  }
};

/*
    Конструктор / деструктор
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const SceneRender& render)
  : impl (render.impl)
{
  addref (impl);
}

SceneRender::~SceneRender ()
{
  release (impl);
}

SceneRender& SceneRender::operator = (const SceneRender& render)
{
  SceneRender (render).Swap (*this);
  
  return *this;
}

/*
    Установка / сброс системы рендеринга
*/

void SceneRender::SetRenderManager (render::RenderManager* manager)
{
  try
  {
    if (manager == impl->manager)
      return;
      
    impl->manager = 0;
      
    impl->Destroy ();
    
    impl->manager = manager;    

    try
    {
      if (manager)
      {
        try
        {
          impl->destroy_manager_connection = get_trackable (*manager).connect_tracker (impl->on_destroy_manager);
        }
        catch (...)
        {
          impl->manager = 0;
          throw;
        }
      }
      
      impl->Initialize ();
    }
    catch (...)
    {
      impl->destroy_manager_connection.disconnect ();

      impl->manager = 0;

      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::SetRenderManager");
    throw;
  }
}

render::RenderManager* SceneRender::RenderManager () const
{
  return impl->manager;
}

/*
    Проверка связи цели рендеринга с рендером
*/

bool SceneRender::IsBindedToRenderer () const
{
  return impl->manager && impl->screen;
}

/*
    Экран (политика владения - weak-ref)
*/

void SceneRender::SetScreen (scene_graph::Screen* screen)
{
  try
  {
    if (screen == impl->screen)
      return;
      
    if (impl->screen)
    {
      try
      {
        impl->screen->DetachListener (impl);
      }
      catch (...)
      {
      }
    }

    impl->screen = 0;

    impl->Destroy ();

    impl->screen = screen;

    if (screen)
      screen->AttachListener (impl);

    try
    {
      impl->Initialize ();
    }
    catch (...)
    {
      impl->screen = 0;
      
      if (screen)
        screen->DetachListener (impl);
      
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::SetScreen");
    throw;
  }
}

scene_graph::Screen* SceneRender::Screen () const
{
  return impl->screen;
}

/*
    Регистрация целевых буферов отрисовки
*/

void SceneRender::SetRenderTarget (const char* name, const render::RenderTarget& target)
{
  impl->SetRenderTarget (name, target);
}

void SceneRender::RemoveRenderTarget (const char* name)
{
  impl->RemoveRenderTarget (name);
}

void SceneRender::RemoveAllRenderTargets ()
{
  impl->RemoveAllRenderTargets ();
}

/*
    Получение целевых буферов отрисовки
*/

bool SceneRender::HasRenderTarget (const char* name) const
{
  if (!name)
    return false;

  return impl->render_targets.find (name) != impl->render_targets.end ();
}

RenderTarget SceneRender::RenderTarget (const char* name) const
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    RenderTargetMap::iterator iter = impl->render_targets.find (name);
    
    if (iter == impl->render_targets.end ())
      throw xtl::make_argument_exception ("", "name", name, "Render target not found");
      
    return iter->second;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::RenderTarget");
    throw;
  }
}

/*
    Обновление
*/

void SceneRender::Update (Frame* parent_frame)
{
  try
  {
      //пересчёт расположение областей вывода
      
    if (impl->need_sort_views)
      impl->SortViews ();
      
      //обновление
    
    for (ViewList::iterator iter=impl->views.begin (), end=impl->views.end (); iter!=end; ++iter)    
    {
      try
      {
        (*iter)->UpdateFrame (parent_frame);
      }
      catch (std::exception& e)
      {
        impl->log.Printf ("%s\n    at render::SceneRender::Update", e.what ());
      }
      catch (...)
      {
        impl->log.Printf ("unknown exception\n    at render::SceneRender::Update");
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRender::Update");
    throw;
  }
}

/*
    Обмен
*/

void SceneRender::Swap (SceneRender& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

void swap (SceneRender& render1, SceneRender& render2)
{
  render1.Swap (render2);
}

}
