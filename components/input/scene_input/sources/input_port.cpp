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
  , touch_size (1.0f)
  , touch_size_space (InputTransformSpace_Default)
{
  static const size_t PLANES_COUNT = 6;

  touch_frustum.reserve (PLANES_COUNT);

  z_order_changed = true;    

  OnViewportChangeCamera (viewport.Camera ());
  
  viewport.AttachListener (this);  
}

InputPort::~InputPort ()
{
  if (input_scene)
    input_scene->RemoveAllTouches (*this);

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
  
  if (input_scene && &input_scene->Scene () != scene)
    input_scene->RemoveAllTouches (*this);

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
    Размер тача
*/

void InputPort::SetTouchSize (float size, InputTransformSpace space)
{
  touch_size       = size;
  touch_size_space = space;
  need_update      = true;
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
                
    view_proj_tm           = math::scale (math::vec3f (1.0f, 1.0f, -1.0f)) * camera->ProjectionMatrix () * math::inverse (camera->WorldTM ());                
    inv_view_proj_tm       = math::inverse (view_proj_tm);
    normalized_position_tm = math::scale (math::vec3f (1.0f, -1.0f, 1.0f)) * math::translate (math::vec3f (-1.0f)) * math::scale (2.0f * viewport_scale) * math::translate (-viewport_offset);
    position_tm            = inv_view_proj_tm * normalized_position_tm;
    screen_tm              = math::inverse (normalized_position_tm) * view_proj_tm;
    
    switch (touch_size_space)
    {
      case InputTransformSpace_Screen:
        touch_scale = math::vec3f (1.0f) / (math::vec3f (touch_size, touch_size, 1.0f) * viewport_scale);
        break;
      case InputTransformSpace_Camera:
      {
        math::vec4f normalized_scale = camera->ProjectionMatrix () * math::vec4f (touch_size, touch_size, 0.0f, 0.0f);
        
        touch_scale = abs (math::vec3f (2.0f / normalized_scale.x, 2.0f / normalized_scale.y, 1.0f));        

        break;
      }
    }        

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

void InputPort::OnTouch (TouchProcessingContext& touch_context, const math::vec3f& touch_world_position)
{
  try
  {
    if (!input_scene || !is_active)
      return;
    
      //оповещение сцены о возникновении события

    InputPortPtr self_lock (this);      
      
    input_scene->OnTouch (*this, touch_world_position, touch_context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputPort::OnTouch");
    throw;
  }
}

void InputPort::FindTouch (TouchProcessingContext& touch_context, math::vec3f& out_touch_world_position)
{
  try
  {
    if (!input_scene || !is_active)
      return;
    
    if (need_update)
      Update ();

      //перевод координаты в мировую систему координат

    math::vec4f source_position (touch_context.event.position.x, touch_context.event.position.y, 0.0f, 1.0f),
                world_position = position_tm * source_position;

    world_position /= world_position.w;    

      //получение пирамиды тача
      
    math::vec4f normalized_position = normalized_position_tm * source_position, normalized_direction (0.0f, 0.0f, 2.0f, 0.0f);

    normalized_position /= normalized_position.w;
    
    math::vec4f world_direction = inv_view_proj_tm * normalized_direction;
    
    math::mat4f touch_tm = math::scale (touch_scale) * math::translate (math::vec3f (-normalized_position.x, -normalized_position.y)) * view_proj_tm;

    touch_frustum.clear ();

    add_frustum (touch_tm, touch_frustum);

      //поиск зоны
      
    out_touch_world_position = math::vec3f (world_position);

    input_scene->FindTouch (*this, out_touch_world_position, math::vec3f (world_direction), touch_frustum, screen_tm, touch_context);        
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputPort::FindTouch");
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
