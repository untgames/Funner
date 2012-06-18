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
