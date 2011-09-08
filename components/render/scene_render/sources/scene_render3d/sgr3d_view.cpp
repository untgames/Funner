#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    Константы
*/

const size_t TECHNIQUE_ARRAY_RESERVE = 8; //резервируемое количество техник рендеринга

}

/*
    Описание реализации рендера
*/

typedef stl::vector<TechniquePtr> TechniqueArray;

struct View::Impl
{
  RenderPtr            render;                    //рендер
  stl::string          technique_name;            //имя техники рендеринга
  TechniqueArray       techniques;                //техники
  render::Frame        frame;                     //кадр
  scene_graph::Camera* camera;                    //текущая камера
  common::PropertyMap  properties;                //свойства рендеринга
  ScenePtr             scene;                     //текущая сцена
  bool                 need_update_properties;    //необходимо обновить свойства рендеринга
  bool                 need_update_camera;        //необходимо обновить камеру
  Log                  log;                       //поток протоколирования

///Конструктор
  Impl (RenderManager& in_manager, const char* in_technique)
    : render (Render::GetRender (in_manager))
    , technique_name (in_technique)
    , frame (in_manager.CreateFrame ())
    , camera ()
    , need_update_properties (true)
    , need_update_camera (true)
  {
    techniques.reserve (TECHNIQUE_ARRAY_RESERVE);
    
    log.Printf ("View created for technique '%s'", technique_name.c_str ());
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("View destroyed for technique '%s'", technique_name.c_str ());
  }
};

/*
    Конструктор / деструктор
*/

View::View (RenderManager& manager, const char* technique)
{
  try
  {
    if (!technique)
      throw xtl::make_null_argument_exception ("", "technique");
    
    impl = new Impl (manager, technique);
    
    impl->render->RegisterView (*this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::View::View");
    throw;
  }
}

View::~View ()
{
  try
  {
    impl->render->UnregisterView (*this);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Кадр
*/

render::Frame& View::Frame ()
{
  return impl->frame;
}

/*
    Обновление камеры
*/

void View::UpdateCamera (scene_graph::Camera* camera)
{
  impl->camera             = camera;
  impl->need_update_camera = true;
}

/*
    Обновление свойств ренедринга
*/

void View::UpdateProperties (const common::PropertyMap& properties)
{
  impl->properties             = properties;
  impl->need_update_properties = true;
}

/*
    Обновление содержимого кадра
*/

void View::UpdateFrame ()
{
  try
  {
      //проверка необходимости обновления свойств рендеринга
      
    if (impl->need_update_properties)
    {
      for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
        (*iter)->UpdateProperties (impl->properties);

      impl->need_update_properties = false;
    }
    
      //проверка необходимости обновления камеры
      
    if (impl->need_update_camera)
    {
        //обновление сцены
        
      if (impl->camera && impl->camera->Scene ())
      {
        impl->scene = impl->render->GetScene (*impl->camera->Scene ());
      }
      else
      {
        impl->scene = ScenePtr ();
      }
        
        //оповещение техник
      
      for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
        (*iter)->UpdateCamera (impl->camera);

      impl->need_update_camera = false;
    }        
    
      //обновление кадра
      
    if (impl->scene)
    {
      for (TechniqueArray::iterator iter=impl->techniques.begin (), end=impl->techniques.end (); iter!=end; ++iter)
        (*iter)->UpdateFrame (*impl->scene, impl->frame);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render:scene_graph3d::View::UpdateFrame");
    throw;
  }
}

/*
    Подсчёт ссылок
*/

void View::AddRef ()
{
  addref (this);
}

void View::Release ()
{
  release (this);
}

/*
    Имя техники рендеринга
*/

const char* View::TechniqueName ()
{
  return impl->technique_name.c_str ();
}

/*
    Управление техниками рендеринга
*/

void View::AddTechnique (const TechniquePtr& technique)
{
  impl->techniques.push_back (technique);
}

void View::RemoveAllTechniques ()
{
  impl->techniques.clear ();
}
