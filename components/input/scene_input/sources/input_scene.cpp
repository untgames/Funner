#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    Конструктор / деструктор
*/

InputScene::InputScene (scene_graph::Scene& in_scene)
  : scene (in_scene)
{
}

InputScene::~InputScene ()
{
  entities.clear ();
}

/*
    Сброс состояния нажатий
*/

void InputScene::ResetTouchState ()
{
  entities.clear ();
}

/*
    Получение объекта ввода
*/

InputEntityPtr InputScene::GetEntity (const scene_graph::InputZoneModel& zone)
{
  try
  {
    EntityMap::iterator iter = entities.find (&zone);
    
    if (iter != entities.end ())
      return iter->second.entity;

    EntityDesc& desc = entities [&zone];
    
    try
    {
      desc.entity                = InputEntityPtr (new InputEntity (zone), false);
      desc.on_destroy_connection = zone.RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&InputScene::OnInputZoneDestroyed, this, &zone));
      
      return desc.entity;
    }
    catch (...)
    {
      entities.erase (&zone);
      throw;
    }      
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputScene::GetEntity");
    throw;
  }
}

/*
    Оповещение об удалении зоны ввода
*/

void InputScene::OnInputZoneDestroyed (const scene_graph::InputZoneModel* zone)
{
  entities.erase (zone);
}

/*
    Обработка события нажатия
*/

namespace
{

struct TouchTraverser: public INodeTraverser
{
  InputScene&        scene;
  const TouchEvent&  event;
  const math::vec3f& touch_world_position;
  bool&              touch_catched;
  
  TouchTraverser (InputScene& in_scene, const TouchEvent& in_event, const math::vec3f& in_touch_world_position, bool& in_touch_catched)
    : scene (in_scene)
    , event (in_event)
    , touch_world_position (in_touch_world_position)
    , touch_catched (in_touch_catched)
  {
  }

  void operator () (Node& node)
  {
    scene_graph::InputZoneModel* zone = dynamic_cast<scene_graph::InputZoneModel*> (&node);

    if (!zone)
      return;

//    scene.OnTouch (*zone, event, touch_world_position, touch_catched);
  }
};

}

void InputScene::OnTouch (const TouchEvent& event, const math::vec3f& touch_world_position, const frustum& touch_frustum, bool& touch_catched)
{
  try
  {
    TouchTraverser traverser (*this, event, touch_world_position, touch_catched);
    
    scene.Traverse (touch_frustum, traverser);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputScene::OnTouch(const TouchEvent&,const math::vec3f&,const frustum&,bool&)");
    throw;
  }  
}
    