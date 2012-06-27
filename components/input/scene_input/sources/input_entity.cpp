#include "shared.h"

using namespace scene_graph;
using namespace input;

namespace
{

const int RESERVE_TOUCHES_COUNT = 5; //резервируемое число одновременных нажатий дл€ обработки

}

/*
     онструктор / деструктор
*/

InputEntity::InputEntity (const scene_graph::InputZoneModel& in_zone, InputScene& in_scene)
  : zone (in_zone)
  , scene (in_scene)
  , has_screen_handlers (false)
  , wait_for_release_event (false)
{
  touches.reserve (RESERVE_TOUCHES_COUNT);

  on_notifications_changed_connection = zone.RegisterEventHandler (InputZoneEvent_AfterNotificationAdded, xtl::bind (&InputEntity::UpdateNotifications, this));
  
  UpdateNotifications ();
}

InputEntity::~InputEntity ()
{
}

/*
    ¬едение таблицы нажатий
*/

void InputEntity::AddTouch (touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->touch == touch && iter->button == button)
      return;
      
  touches.push_back (TouchDesc (touch, button));
  
  wait_for_release_event = true;
  
  UpdateBroadcasts ();
}

InputEntity::TouchDesc* InputEntity::FindTouch (touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->touch == touch && iter->button == button)
      return &*iter;
      
  return 0;
}

void InputEntity::RemoveTouch (touch_t touch, int button)
{
  for (TouchDescArray::iterator iter=touches.begin (), end=touches.end (); iter!=end; ++iter)
    if (iter->touch == touch && iter->button == button)
    {
      touches.erase (iter);
      
      wait_for_release_event = !touches.empty ();
      
      UpdateBroadcasts ();
      
      return;
    }
}

/*
    ќбработка событи€
*/

void InputEntity::OnTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position, size_t zone_index, const math::vec2f& touch_local_position)
{
  try
  {
      //формирование контекста событи€
      
    InputZoneNotificationContext context;

    context.touch_id             = event.touch;
    context.button               = event.button;
    context.touch_world_position = touch_world_position;
    context.touch_local_position = touch_local_position;
    
      //проверка: зафиксировано ли нажатие на зону

    TouchDesc* touch_desc = FindTouch (event.touch, event.button);    
    
    if (touch_desc && touch_desc->is_inside)
    {
      touch_desc->touch_check = true;
      
      switch (event.state)
      {
        case TouchState_Moving:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchMove, context);      
          break;
        case TouchState_Pressed:
          //ignored
          break;
        case TouchState_Released:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpInside, context);
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchClick, context);
          
          RemoveTouch (event.touch, event.button);

          break;
        default:
          break;        
      }
    }
    else if (touch_desc)
    {
      touch_desc->is_inside   = true;
      touch_desc->touch_check = true;
      
      switch (event.state)
      {
        case TouchState_Moving:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchEnter, context);
          break;
        case TouchState_Pressed:
          //ignored
          break;
        case TouchState_Released:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpInside, context);
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchClick, context);
          break;
        default:
          break;        
      }        
    }
    else
    {      
      switch (event.state)
      {
        case TouchState_Moving:
          AddTouch (event.touch, event.button);              
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchEnter, context);
          break;
        case TouchState_Pressed:
          AddTouch (event.touch, event.button);              
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchDown, context);
          break;
        case TouchState_Released:
          zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpInside, context);
          break;
        default:
          break;        
      }        
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::OnTouch");
    throw;
  }
}

void InputEntity::OnBroadcastTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position)
{
  try
  {
    if (!has_screen_handlers && !wait_for_release_event)
      return;
      
      //подготовка контекста оповещени€

    InputZoneNotificationContext context;

    context.touch_id             = event.touch;
    context.button               = event.button;
    context.touch_world_position = touch_world_position;
    
      //экранные оповещени€

    switch (event.state)
    {
      case TouchState_Moving:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnScreenTouchMove, context);
        break;
      case TouchState_Pressed:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnScreenTouchDown, context);
        break;
      case TouchState_Released:
        zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnScreenTouchUp, context);
        break;
      default:
        break;
    }
    
      //зависимые оповещени€
      
    TouchDesc* touch_desc = FindTouch (event.touch, event.button);
      
    if (touch_desc)
    {      
      if (!touch_desc->touch_check)
      {
        switch (event.state)
        {
          case TouchState_Moving:
            if (touch_desc->is_inside)
            {
              zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchLeave, context);
              touch_desc->is_inside = false;
            }
            
            break;
          case TouchState_Pressed:
            //ignored
            break;
          case TouchState_Released:
            zone.Notify (input_port.AttachedViewport (), InputZoneNotification_OnTouchUpOutside, context);
            RemoveTouch (event.touch, event.button);
            break;
          default:
            break;
        }      
      }
      
      touch_desc->touch_check = false;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::OnBroadcastTouch");
    throw;
  }
}

void InputEntity::UpdateBroadcasts ()
{
  if (IsAttached ())
  {
    if (!(wait_for_release_event || has_screen_handlers))
      Detach ();
  }
  else
  {
    if (wait_for_release_event || has_screen_handlers)
      scene.RegisterBroadcastListener (*this);
  }
}

/*
    ќбновление списка оповещений
*/

void InputEntity::UpdateNotifications ()
{
  try
  {
      //проверка наличи€ экранных обработчиков
    
    has_screen_handlers  = false;
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchDown);
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchUp);
    has_screen_handlers |= zone.HasNotificationHandler (InputZoneNotification_OnScreenTouchMove);

      //обновление подписки на броадкасты
      
    UpdateBroadcasts ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputEntity::UpdateNotifications");
    throw;
  }
}
