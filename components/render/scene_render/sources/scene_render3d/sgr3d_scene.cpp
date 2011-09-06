#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

//TODO: scene trackable

/*
    Описание реализации сцены рендеринга
*/

struct Scene::Impl
{
  RenderPtr           render; //рендер
  scene_graph::Scene* scene;  //исходная сцена
  Log                 log;    //поток протоколирования
  
///Конструктор
  Impl (scene_graph::Scene& in_scene, const RenderPtr& in_render)
    : render (in_render)
    , scene (&in_scene)
  {
    log.Printf ("Scene created");    
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("Scene destroyed");
  }
};

/*
    Конструктор / деструктор
*/

Scene::Scene (scene_graph::Scene& scene, const RenderPtr& render)
{
  try
  {
    if (!render)
      throw xtl::make_null_argument_exception ("", "render");    
    
    impl = new Impl (scene, render);
    
    render->RegisterScene (scene, *this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Scene::Scene");
    throw;
  }
}

Scene::~Scene ()
{
  try
  {
    impl->render->UnregisterScene (*impl->scene);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}
