#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    Конструктор / деструктор
*/

InputPort::InputPort (Viewport& in_viewport, bool& in_z_order_changed)
  : viewport (in_viewport)
  , z_order_changed (in_z_order_changed)
  , is_active (viewport.IsActive () && viewport.InputState ())
  , need_update (true)
{
  z_order_changed = true;    

  OnViewportChangeCamera (viewport.Camera ());
  
  viewport.AttachListener (this);
}

InputPort::~InputPort ()
{
  viewport.DetachListener (this);
}

/*
    Обработчики событий
*/

void InputPort::OnViewportChangeArea (const Rect&, float, float)
{
  need_update = true;
}

void InputPort::OnSceneChanged ()
{
  need_update = true;
  
  Camera* camera = viewport.Camera ();
  Scene*  scene  = camera ? camera->Scene () : (Scene*)0;

  if (!scene)
  {    
    input_scene = InputScenePtr ();    
    return;
  }
  
  input_scene = InputSceneCollectionSingleton::Instance ()->GetScene (*scene);
}

void InputPort::OnViewportChangeCamera (Camera* new_camera)
{
  need_update = true;
  
  on_camera_updated.disconnect ();
  on_scene_changed.disconnect ();
  
  OnSceneChanged ();  

  if (!new_camera)
    return;

  on_camera_updated = new_camera->RegisterEventHandler (NodeEvent_AfterUpdate, xtl::bind (&InputPort::OnCameraUpdated, this));
  on_scene_changed  = new_camera->RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&InputPort::OnSceneChanged, this));
}

void InputPort::OnCameraUpdated ()
{
  need_update = true;
}

void InputPort::OnViewportChangeZOrder (int)
{
  z_order_changed = true;
}

void InputPort::OnViewportChangeActive (bool)
{
  is_active = viewport.IsActive () && viewport.InputState ();
}

void InputPort::OnViewportChangeInputState (bool)
{
  is_active = viewport.IsActive () && viewport.InputState ();
}

/*
    Обновление параметров области ввода
*/

void InputPort::Update ()
{  
  if (!need_update)
    return;        
    
  try
  {    
    Camera* camera = viewport.Camera ();
    
    if (!camera || !input_scene)
    {
      need_update = false;
      return;
    }
    
    const scene_graph::Rect& viewport_rect = viewport.Area ();

    math::vec3f viewport_offset (float (viewport_rect.left ()), float (viewport_rect.top ()), 0.0f),
                viewport_scale (1.0f / float (viewport_rect.right () - viewport_rect.left ()), 1.0f / float (viewport_rect.bottom () - viewport_rect.top ()), 1.0f);                
                
    position_tm = math::inverse (camera->ProjectionMatrix () * math::inverse (camera->WorldTM ())) * math::translate (math::vec3f (-1.0f)) * 
      math::scale (2.0f * viewport_scale) * math::translate (-viewport_offset);

    ////????????????????????

    need_update = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputPort::Update");
    throw;
  }
}

/*
    Обработка события нажатия
*/

void InputPort::OnTouch (const TouchEvent& event, bool& touch_catched)
{
  try
  {
    if (need_update)
      Update ();      

    math::vec4f position = position_tm * math::vec4f (event.position.x, event.position.y, 0.0f, 1.0f);
    
    position /= position.w;    
        
    printf ("position is %f %f -> %f %f %f %f\n", event.position.x, event.position.y, position.x, position.y, position.z, position.w);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputPort::OnTouch");
    throw;
  }
}

/*
    Сброс состояния нажатий
*/

void InputPort::ResetTouchState ()
{
  if (!input_scene)
    return;
    
  input_scene->ResetTouchState ();
}
